#include <Ws2tcpip.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "infinispan/hotrod/exceptions.h"
#include "hotrod/sys/Log.h"
#include "hotrod/sys/Socket.h"


namespace infinispan {
namespace hotrod {
namespace sys {

// Part of a straw man IO layer



namespace windows {

static void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

static int getPreferredIPStack() {
    const char *stack = getenv("HOTROD_IPSTACK");
    if (stack == 0) {
        return AF_UNSPEC;
    }  else if (!_stricmp(stack, "IPV4")) {
        return AF_INET;
    } else if (!_stricmp(stack, "IPV6")) {
        return AF_INET6;
    } else {
        throw std::runtime_error("Invalid HOTROD_IPSTACK environment variable");
    }
}

static int preferredIPStack = getPreferredIPStack();

class Socket: public infinispan::hotrod::sys::Socket {
  public:
    Socket();
    virtual void connect(const std::string& host, int port, int timeout);
    virtual void close();
    virtual void setTcpNoDelay(bool tcpNoDelay);
    virtual void setTimeout(int timeout);
    virtual size_t read(char *p, size_t n);
    virtual void write(const char *p, size_t n);
  private:
    SOCKET fd;
    std::string host;
    int port;
    static bool started;
};

namespace {

void throwIOErr (const std::string& host, int port, const char *msg, int errnum) {
    std::string m(msg);
    if (errnum != 0) {
        char buf[200];
        if (strerror_s(buf, 200, errnum) == 0) {
            m += " ";
            m += buf;
        }
        else {
            m += " ";
            m += strerror(errnum);
        }
    }
    throw TransportException(host, port, m);
}

} /* namespace */

bool Socket::started = false;

Socket::Socket() : fd(INVALID_SOCKET) {
    if (!started) {
        /* Request WinSock 2.2 */
        WORD wsa_ver = MAKEWORD(2, 2);
        WSADATA unused;
        int err = WSAStartup(wsa_ver, &unused);
        if (err)
            throwIOErr("", 0, "windows WSAStartup failed", err);
        started = true;
    }
}

void Socket::connect(const std::string& h, int p, int timeout) {
    struct addrinfo hints;
    struct addrinfo *addr, *addr_list;
    char ip[INET6_ADDRSTRLEN];
    int error, flags;
    u_long non_blocking;
    SOCKET sock;

    host = h;
    port = p;
    if (fd != INVALID_SOCKET) throwIOErr(host, port, "reconnect attempt", 0);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = preferredIPStack;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = 0;

    std::ostringstream ostr;
    ostr << port;

    int ec = getaddrinfo(host.c_str(), ostr.str().c_str(), &hints, &addr_list);
    if (ec) throwIOErr(host, port,"Error while invoking getaddrinfo", WSAGetLastError());

    // Cycle through all returned addresses
    for(addr = addr_list; addr != NULL; addr = addr->ai_next ) {
        inet_ntop(addr->ai_family, get_in_addr((struct sockaddr *)addr->ai_addr), ip, sizeof(ip));
        sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sock == INVALID_SOCKET) {
            DEBUG("Failed to obtain socket for address family %d", addr->ai_family);
            continue;
        }

        // Make the socket non-blocking for the connection
        non_blocking = 1;
        ioctlsocket(sock, FIONBIO, &non_blocking);

        // Connect
        DEBUG("Attempting connection to %s:%d", ip, port);
        int s = ::connect(sock, addr->ai_addr, addr->ai_addrlen);
        error = WSAGetLastError();


        if (s < 0) {
            if (error == WSAEWOULDBLOCK) {
                struct timeval tv;
                tv.tv_sec = timeout / 1000;
                tv.tv_usec = timeout % 1000;
                fd_set sock_set;
                FD_ZERO(&sock_set);
                FD_SET(sock, &sock_set);
                // Wait for the socket to become ready
                s = select(sock + 1, NULL, &sock_set, NULL, &tv);
                if (s > 0) {
                    int opt;
                    socklen_t optlen = sizeof(opt);
                    s = getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)(&opt), &optlen);
                } else if (s == 0) {
                    close();
                    DEBUG("Timed out connecting to %s:%d", ip, port);
                    continue;
                } else {
                    error = WSAGetLastError();
                }
            } else {
                s = -1;
            }
        }
        if (s < 0) {
            close();
        } else {
            // We got a successful connection
            break;
        }
    }
    freeaddrinfo(addr_list);
    /* No address succeeded */
    if(addr == NULL) {
        throwIOErr(host, port, "Failed to connect", error);
    }
    DEBUG("Connected to %s:%d", ip, port);
    // Set to blocking mode again
    non_blocking = 0;
    ioctlsocket(sock, FIONBIO, &non_blocking);
    fd = sock;
}

void Socket::close() {
    ::closesocket(fd);
    fd = INVALID_SOCKET;
}

void Socket::setTcpNoDelay(bool tcpNoDelay) {
    int flag = tcpNoDelay;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char *) &flag, sizeof(flag)) < 0) {
        throwIOErr(host, port, "Failure setting TCP_NODELAY", WSAGetLastError());
    }
}

void Socket::setTimeout(int timeout) {
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = timeout % 1000;

    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(tv)) < 0) {
        throwIOErr(host, port, "Failure setting receive socket timeout", WSAGetLastError());
    }

    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char *) &tv, sizeof(tv)) < 0) {
        throwIOErr(host, port, "Failure setting send socket timeout", WSAGetLastError());
    }
}

size_t Socket::read(char *p, size_t length) {
    while(1) {
        ssize_t n =  recv(fd, p, (int) length, 0);
        if (n == SOCKET_ERROR)
            throwIOErr(host, port, "read", WSAGetLastError());
        else if (n == 0)
            return 0;
        else
            return n;
    }
}

void Socket::write(const char *p, size_t length) {
    ssize_t n = send(fd, p, (int) length, 0);
    if (n == SOCKET_ERROR) throwIOErr (host, port, "write", WSAGetLastError());
    if ((size_t) n != length) throwIOErr (host, port, "write error incomplete", 0);
}

} /* windows namespace */


Socket* Socket::create() {
    return new windows::Socket();
}

}}} /* namespace */
