#ifndef ISPN_HOTROD_TRANSPORT_TCP_INPUTSTREAM_H
#define ISPN_HOTROD_TRANSPORT_TCP_INPUTSTREAM_H

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

#include "hotrod/sys/types.h"
#include "hotrod/sys/Socket.h"

#include <sstream>

namespace infinispan {
namespace hotrod {
namespace transport {

class InputStream
{
  public:
    void read(char* buffer, size_t size);
    char read();
  private:
    static const size_t BufferSize = 1024;
    InputStream(sys::Socket& socket);
    sys::Socket& socket;
    char buffer[BufferSize];
    char* ptr;
    bool hasMore;
    size_t capacity;

  friend class Socket;
};

class OutputStream
{
  public:
    void write(const char *p, size_t n);
    void write(char c);
    void flush();
  private:
    OutputStream(sys::Socket& socket);
    sys::Socket& socket;
    std::ostringstream out;

  friend class Socket;
};

class Socket
{
  public:
    Socket();
    void connect(const std::string& host, int port);
    void close();
    InputStream& getInputStream();
    OutputStream& getOutputStream();

  private:
    sys::Socket& socket;
    InputStream inputStream;
    OutputStream outputStream;

};

}}} // namespace infinispan::hotrod::transport::tcp

#endif  // ISPN_HOTROD_TRANSPORT_TCP_INPUTSTREAM_H

