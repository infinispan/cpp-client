#include "infinispan/hotrod/exceptions.h"
#include "hotrod/sys/Socket.h"

#include <winsock2.h>
#include <Ws2tcpip.h>

#include <iostream>
#include <sstream>

namespace infinispan {
namespace hotrod {
namespace sys {

// Part of a straw man IO layer



namespace windows {

class Socket: public infinispan::hotrod::sys::Socket {
  public:
    Socket();
    virtual void connect(const std::string& host, int port);
    virtual void close();
    virtual size_t read(char *p, size_t n);
    virtual void write(const char *p, size_t n);
  private:
    SOCKET fd;
    std::string host;
    int port;
    static bool started;
};

namespace {
// TODO: centralized hotrod exceptions with file name and line number
void throwIOErr (const std::string& host, int port, const char *msg, int errnum) {
    std::string m(msg);
    if (errno != 0) {
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

void Socket::connect(const std::string& h, int p) {
    host = h;
    port = p;
    if (fd != INVALID_SOCKET) throwIOErr(host, port, "reconnect attempt", 0);
    SOCKET sock = socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
    if (sock == INVALID_SOCKET) throwIOErr(host, port,"connect", errno);

    struct addrinfo *addr;
    std::ostringstream ostr;
    ostr << port;
    int ec = getaddrinfo(host.c_str(), ostr.str().c_str(), NULL, &addr);
    if (ec) throwIOErr(host, port, "getaddrinfo", WSAGetLastError());

    if (::connect(sock, addr->ai_addr, (int)addr->ai_addrlen) != 0) {
        int wsaerr =  WSAGetLastError();
        freeaddrinfo(addr);
        close();
        throwIOErr(host, port,"connect2", wsaerr);
    }

    freeaddrinfo(addr);
    fd = sock;
}

void Socket::close() {
    ::closesocket(fd);
    fd = INVALID_SOCKET;
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
