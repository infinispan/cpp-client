

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

//For OSX portability
int sendFlag = 0;

namespace infinispan {
namespace hotrod {
namespace sys {

// Part of a straw man IO layer



namespace posix {

class Socket: public infinispan::hotrod::sys::Socket {
  public:
    Socket();

    virtual ~Socket();
    virtual void connect(const std::string& host, int port);
    virtual void close();
    virtual size_t read(char *p, size_t n);
    virtual void write(const char *p, size_t n);
  private:
    int fd;
    std::string host;
    int port;
};

namespace {
// TODO: centralized hotrod exceptions file name + line number
void throwIOErr (const std::string& host, int port, const char *msg, int errnum) {
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
    throw TransportException(host, port, m);
}

} /* namespace */

Socket::Socket() : fd(-1), port(-1) {}

Socket::~Socket() { close(); }

void Socket::connect(const std::string& h, int p) {
	host = h;
	port = p;
    if (fd != -1) throwIOErr(host, port, "reconnect attempt", 0);
    int sock = socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
    if (sock == -1) throwIOErr(host, port,"connect", errno);

    //OSX portability as MSG_NOSIGNAL is not defined on OSX
    #ifdef MSG_NOSIGNAL
        sendFlag = MSG_NOSIGNAL;
    #else
        sendFlag = 0;
        int optval = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval)) == -1) {
        	    close();
        	    throwIOErr(host, port, "setsockopt", errno);
        }
    #endif

    struct addrinfo *addr;
    std::ostringstream ostr;
    ostr << port;
    int ec = getaddrinfo(host.c_str(), ostr.str().c_str(), NULL, &addr);
    if (ec) throwIOErr(host, port,"getaddrinfo", errno);

    while (::connect(sock, addr->ai_addr, addr->ai_addrlen) == -1) {
        int connect_errno = errno;
        if (connect_errno != EINPROGRESS) {
            freeaddrinfo(addr);
            close();
            throwIOErr(host, port,"connect2", connect_errno);
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
            throwIOErr(host, port,"read", errno);
        else if (n == 0)
            throwIOErr(host, port,"no read", 0);
            //return 0;
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
