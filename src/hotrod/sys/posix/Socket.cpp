#include "infinispan/hotrod/exceptions.h"
#include "hotrod/sys/Socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>

#include <errno.h>
#include <stdexcept>
#include <string.h>

#include <iostream>
#include <istream>
#include <sstream>

namespace infinispan {
namespace hotrod {
namespace sys {

// Part of a straw man IO layer



namespace posix {

class Socket: public infinispan::hotrod::sys::Socket {
  public:
    Socket();
    virtual void connect();
    virtual void close();
    virtual size_t read(char *p, size_t n);
    virtual void write(const char *p, size_t n);
  private:
    int fd;
};

namespace {
// TODO: centralized hotrod exceptions file name + line number
void throwIOErr (const char *msg, int errnum) {
    std::string m(msg);
    if (errno != 0) {
        char buf[200];
        if (strerror_r(errnum, buf, 200) == 0) {
            m += " ";
            m += buf;
        }
        else {
            m += " ";
            m += strerror(errnum);
        }
    }
    throw TransportException(m);
}

} /* namespace */

Socket::Socket() : fd(-1) {}

void Socket::connect() {
    if (fd != -1) throwIOErr("reconnect attempt", 0);
    int sock = socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
    if (sock == -1) throwIOErr("connect", errno);

    struct addrinfo *addr;
    int ec = getaddrinfo("127.0.0.1", "11222", NULL, &addr);
    if (ec) throwIOErr("getaddrinfo", errno);

    while (::connect(sock, addr->ai_addr, addr->ai_addrlen) == -1) {
        if (errno != EINPROGRESS) {
            freeaddrinfo(addr);
            close();
            throwIOErr("connect2", errno);
        }
    }

    freeaddrinfo(addr);
    fd = sock;
}

void Socket::close() {
    ::close(fd);
    fd = -1;
}

size_t Socket::read(char *p, size_t length) {
    while(1) {
        ssize_t n =  recv(fd, p, length, 0);
        if (n < 0 && errno != EAGAIN)
            throwIOErr("read", errno);
        else if (n == 0)
            return 0;
        else
            return n;
    }
}

void Socket::write(const char *p, size_t length) {
    ssize_t n = send(fd, p, length, MSG_NOSIGNAL);
    if (n == -1) throwIOErr ("write", errno);
    if ((size_t) n != length) throwIOErr ("write error", 0);
}

} /* posix namespace */


Socket* Socket::create() {
    return new posix::Socket();
}

}}} /* namespace */
