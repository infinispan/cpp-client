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

#include <winsock2.h>
#include <Ws2tcpip.h>

#include <iostream>

namespace infinispan {
namespace hotrod {
namespace sys {

// Part of a straw man IO layer



namespace windows {

class Socket: public infinispan::hotrod::sys::Socket {
  public:
    Socket();
    virtual void connect();
    virtual void close();
    virtual size_t read(char *p, size_t n);
    virtual void write(const char *p, size_t n);
  private:
    SOCKET fd;
    static bool started;
};

namespace {
// TODO: centralized hotrod exceptions with file name and line number
void throwIOErr (const char *msg, int errnum) {
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
    throw TransportException(m);
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
            throwIOErr("windows WSAStartup failed", err);
        started = true;
    }
}

void Socket::connect() {
    if (fd != -1) throwIOErr("reconnect attempt", 0);
    SOCKET sock = socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
    if (sock == INVALID_SOCKET) throwIOErr("connect", errno);

    struct addrinfo *addr;
    int ec = getaddrinfo("127.0.0.1", "11222", NULL, &addr);
    if (ec) throwIOErr("getaddrinfo", errno);

    if (::connect(sock, addr->ai_addr, (int)addr->ai_addrlen) != 0) {
        int wsaerr =  WSAGetLastError();
        freeaddrinfo(addr);
        close();
        throwIOErr("connect2", wsaerr);
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
            throwIOErr("read", WSAGetLastError());
        else if (n == 0)
            return 0;
        else
            return n;
    }
}

void Socket::write(const char *p, size_t length) {
    ssize_t n = send(fd, p, (int) length, 0);
    if (n == SOCKET_ERROR) throwIOErr ("write", WSAGetLastError());
    if ((size_t) n != length) throwIOErr ("write error incomplete", 0);
}

} /* posix namespace */


Socket* Socket::create() {
    return new windows::Socket();
}

}}} /* namespace */
