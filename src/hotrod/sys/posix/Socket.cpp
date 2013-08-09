/*
 * JBoss, Home of Professional Open Source
 * Copyright 2010 Red Hat Inc. and/or its affiliates and other
 * contributors as indicated by the @author tags. All rights reserved.
 * See the copyright.txt in the distribution for a full listing of
 * individual contributors.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */

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

Socket::Socket() : fd(-1) {}

void Socket::connect(const std::string& h, int p) {
	host = h;
	port = p;
    if (fd != -1) throwIOErr(host, port, "reconnect attempt", 0);
    int sock = socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
    if (sock == -1) throwIOErr(host, port,"connect", errno);

    struct addrinfo *addr;
    std::ostringstream ostr;
    ostr << port;
    int ec = getaddrinfo(host.c_str(), ostr.str().c_str(), NULL, &addr);
    if (ec) throwIOErr(host, port,"getaddrinfo", errno);

    while (::connect(sock, addr->ai_addr, addr->ai_addrlen) == -1) {
        if (errno != EINPROGRESS) {
            freeaddrinfo(addr);
            close();
            throwIOErr(host, port,"connect2", errno);
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
            return 0;
        else
            return n;
    }
}

void Socket::write(const char *p, size_t length) {
    ssize_t n = send(fd, p, length, MSG_NOSIGNAL);
    if (n == -1) throwIOErr (host, port,"write", errno);
    if ((size_t) n != length) throwIOErr (host, port,"write error", 0);
}

} /* posix namespace */


Socket* Socket::create() {
    return new posix::Socket();
}

}}} /* namespace */
