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

#include "hotrod/impl/transport/tcp/InputStream.h"
#include "hotrod/sys/Socket.h"

#include <iostream>
#include <cstring>
#include <istream>
#include <sstream>


namespace infinispan {
namespace hotrod {

using sys::Socket;

namespace transport {

// TODO

InputStream::InputStream(sys::Socket& s) : socket(s), ptr(&buffer[0]), hasMore(true), capacity(0)
{}

void InputStream::read(char* buf, size_t size)
{

char* tmp_buffer = buf;
  while (capacity < size) {
    if (capacity != 0) {
      memcpy(tmp_buffer, ptr, capacity);
      tmp_buffer += capacity;
      size -= capacity;
    }
    if (!hasMore)
      throw HotRodClientException("stream not complete");
    capacity = socket.read(&buffer[0],BufferSize);
    ptr = &buffer[0];
    hasMore = capacity < BufferSize ? false : true;
  }
  memcpy(tmp_buffer, ptr, size);
  ptr += size;
  capacity -= size;
}

InputStream& operator>>(InputStream& in, char& c)
{
  in.read(&c,1);
  return in;
}

char InputStream::read()
{
  char c;
  read(&c,1);
  return c;
}

}}} // namespace infinispan::hotrod::transport::tcp
