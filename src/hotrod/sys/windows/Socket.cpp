#include <Ws2tcpip.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "infinispan/hotrod/exceptions.h"
#include "hotrod/sys/Log.h"
#include "hotrod/sys/Inet.h"
#include "hotrod/sys/Socket.h"


namespace infinispan {
namespace hotrod {
namespace sys {

// Part of a straw man IO layer



namespace windows {

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
    std::ostringstream m;
    m << msg;
    m << " (host: " << host;
    m << " port: " << port << ")";

    if (errnum != 0) {
        char buf[200];
        if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errnum, 0, buf, 200, NULL) != 0) {
            m << " " << buf;
        } else {
            m << " Unknown WSA error " << errnum;
        }
    }
    throw TransportException(host, port, m.str());
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
    struct addrinfo *addr, *addr_list;
    char ip[INET6_ADDRSTRLEN];
    int error;
    u_long non_blocking;
    SOCKET sock;

    host = h;
    port = p;
    if (fd != INVALID_SOCKET) throwIOErr(host, port, "reconnect attempt", 0);

    int ec = getaddrinfo(host, port, &addr_list);
    if (ec) {
        std::ostringstream msg;
        msg << "Error while invoking getaddrinfo: " << gai_strerror(ec);
        throwIOErr(host, port, msg.str().c_str(), 0);
    }

    // Cycle through all returned addresses
    bool preferred = true;
    while (true) {
        bool found = false;
        for(addr = addr_list; addr != NULL; addr = addr->ai_next ) {
            inet_ntop(addr->ai_family, get_in_addr((struct sockaddr *)addr->ai_addr), ip, sizeof(ip));
            if ((preferred && (preferredIPStack != addr->ai_family))
                || (!preferred && (preferredIPStack == addr->ai_family))) {
                // Skip non-preferred addresses on the first run and preferred ones on the second one.
                continue;
            }
            TRACE("Trying to connect to %s (%s).", ip, host.c_str());

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
            int s = ::connect(sock, addr->ai_addr, (int) addr->ai_addrlen);
            error = WSAGetLastError();


            if (s < 0) {
                if (error == WSAEWOULDBLOCK) {
                    struct timeval tv;
                    tv.tv_sec = timeout / 1000;
                    tv.tv_usec = timeout % 1000;
                    fd_set sock_set_w;
                    fd_set sock_set_e;
                    FD_ZERO(&sock_set_w);
                    FD_SET(sock, &sock_set_w);
                    FD_ZERO(&sock_set_e);
                    FD_SET(sock, &sock_set_e);
                    // Wait for the socket to become ready
                    s = select((SOCKET)((int) sock + 1), NULL, &sock_set_w, &sock_set_e, &tv);
                    if (s > 0) {
                        if (FD_ISSET(sock, &sock_set_e)) {
                            //Socket in exceptfds set. Connection failed.
                            s = -1;

                            int so_error_value;
                            socklen_t optlen = sizeof(so_error_value);
                            int rcode = getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)(&so_error_value), &optlen);
                            if (SOCKET_ERROR != rcode) {
                                error = so_error_value;
                            }
                        }
                    } else if (s == 0) {
                        ::closesocket(sock);
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
                ::closesocket(sock);
            } else {
                // We got a successful connection
                found = true;
                break;
            }
        }
        if (!found && preferred) {
            // Try the non-preferred addresses.
            preferred = false;
        } else {
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
    DWORD tv = timeout;

    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(tv)) < 0) {
        throwIOErr(host, port, "Failure setting receive socket timeout", WSAGetLastError());
    }

    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char *) &tv, sizeof(tv)) < 0) {
        throwIOErr(host, port, "Failure setting send socket timeout", WSAGetLastError());
    }
}

size_t Socket::read(char *p, size_t length) {
    ssize_t n = recv(fd, p, (int) length, 0);
    if (n != SOCKET_ERROR)
        return n;
    else if (WSAGetLastError() == WSAETIMEDOUT)
        throwIOErr(host, port, "timeout", 0);
    else
        throwIOErr(host, port, "read", WSAGetLastError());
    throw std::exception("Never reached");
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
