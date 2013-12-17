#include "infinispan/hotrod/exceptions.h"
#include "hotrod/sys/Log.h"
#include "hotrod/sys/Inet.h"
#include "hotrod/sys/Socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <poll.h>

#include <errno.h>
#include <string.h>

#include <iostream>
#include <istream>
#include <sstream>
#include <stdexcept>

//For OSX portability
int sendFlag = 0;

namespace infinispan {
namespace hotrod {
namespace sys {

// Part of a straw man IO layer

namespace posix {

static int preferredIPStack = getPreferredIPStack();

class Socket: public infinispan::hotrod::sys::Socket {
  public:
    Socket();

    virtual ~Socket();
    virtual void connect(const std::string& host, int port, int timeout);
    virtual void close();
    virtual void setTcpNoDelay(bool tcpNoDelay);
    virtual void setTimeout(int timeout);
    virtual size_t read(char *p, size_t n);
    virtual void write(const char *p, size_t n);
  private:
    int fd;
    std::string host;
    int port;
};

namespace {

void throwIOErr (const std::string& host, int port, const char *msg, int errnum) {
    std::string m(msg);
    if (errnum != 0) {
        char buf[200];
        if (strerror_r(errnum, buf, 200) == 0) {
            m += " ";
            m += buf;
        } else {
            m += " ";
            m += strerror(errnum);
        }
    }
    throw TransportException(host, port, m);
}

} /* namespace */

Socket::Socket() : fd(-1), port(-1) {}

Socket::~Socket() { close(); }

void Socket::connect(const std::string& h, int p, int timeout) {
    struct addrinfo *addr, *addr_list;
    char ip[INET6_ADDRSTRLEN];
    int error = 0, flags, sock;

    host = h;
    port = p;
    if (fd != -1) throwIOErr(host, port, "reconnect attempt", 0);

    int ec = getaddrinfo(host, port, &addr_list);
    if (ec) throwIOErr(host, port,"Error while invoking getaddrinfo", errno);

    // Cycle through all returned addresses
    for(addr = addr_list; addr != NULL; addr = addr->ai_next ) {
        inet_ntop(addr->ai_family, get_in_addr((struct sockaddr *)addr->ai_addr), ip, sizeof(ip));
        sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sock == -1) {
            DEBUG("Failed to obtain socket for address family %d", addr->ai_family);
            continue;
        }

        // OSX portability as MSG_NOSIGNAL is not defined on OSX
        #ifdef MSG_NOSIGNAL
            sendFlag = MSG_NOSIGNAL;
        #else
            sendFlag = 0;
            int optval = 1;
            if (setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval)) == -1) {
                close();
                DEBUG("Error %d while invoking setsockopt", errno);
                continue;
            }
        #endif


        // Make the socket non-blocking for the connection
        flags = fcntl(sock,F_GETFL,0);
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);

        // Connect
        DEBUG("Attempting connection to %s:%d", ip, port);
        int s = ::connect(sock, addr->ai_addr, addr->ai_addrlen);
        error = errno;

        if (s < 0) {
            if (error == EINPROGRESS) {
                pollfd fds[1];
                fds[0].fd = sock;
                fds[0].events = POLLOUT;
                s = poll(fds, 1, timeout);
                if (s > 0) {
                    if ((POLLOUT ^ fds[0].revents) != 0) {
                        close();
                        DEBUG("Failed to connect to %s:%d", ip, port);
                        continue;
                    }
                    int opt;
                    socklen_t optlen = sizeof(opt);
                    s = getsockopt(sock, SOL_SOCKET, SO_ERROR, (void*)(&opt), &optlen);
                } else if (s == 0) {
                    close();
                    DEBUG("Timed out connecting to %s:%d", ip, port);
                    continue;
                } else {
                    error = errno;
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
    fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);
    fd = sock;
}

void Socket::close() {
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
    }
}

void Socket::setTcpNoDelay(bool tcpNoDelay) {
    int flag = tcpNoDelay;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *) &flag, sizeof(flag)) < 0) {
        throwIOErr(host, port, "Failure setting TCP_NODELAY", errno);
    }
}

void Socket::setTimeout(int timeout) {
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = timeout % 1000;

    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv)) < 0) {
        throwIOErr(host, port, "Failure setting receive socket timeout", errno);
    }

    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv)) < 0) {
        throwIOErr(host, port, "Failure setting send socket timeout", errno);
    }
}

size_t Socket::read(char *p, size_t length) {
    while(1) {
        ssize_t n =  recv(fd, p, length, 0);
        if (n < 0 && errno != EAGAIN)
            throwIOErr(host, port, "read", errno);
        else if (n == 0)
            throwIOErr(host, port, "no read", 0);
        else
            return n;
    }
}

void Socket::write(const char *p, size_t length) {
    ssize_t n = send(fd, p, length, sendFlag);
    if (n == -1) throwIOErr (host, port,"write", errno);
    if ((size_t) n != length) throwIOErr (host, port,"write error", 0);
}

} /* posix namespace */


Socket* Socket::create() {
    return new posix::Socket();
}


}}} /* namespace */
