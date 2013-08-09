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

#include "hotrod/impl/transport/AbstractTransport.h"


namespace infinispan {
namespace hotrod {
namespace transport {

AbstractTransport::AbstractTransport(TransportFactory& tf) :
    transportFactory(tf)
{}

void AbstractTransport::writeArray(const hrbytes& bytes)
{
  hrbytes& not_const_bytes = const_cast<hrbytes&>(bytes);
  writeVInt(not_const_bytes.length());
  writeBytes(bytes);
}

void AbstractTransport::writeLong(int64_t longValue)
{
  hrbytes bytes;
  bytes.reserve(8);
  /*
  for (char* ptr = bytes.bytes() + 8 ; ptr > bytes.bytes() ; --ptr) {
       // TODO: verificare operatore java >>>
       *ptr = (char) longValue >> ( (bytes.bytes() + 8 - ptr)* 8);
  }
  */
  char * ptr = bytes.bytes();
  for (int i = 0 ; i < 8 ; i++) {
    ptr[7-i] = (char) ((longValue) >> (8*i));
  }
  writeBytes(bytes);
}

hrbytes AbstractTransport::readArray()
{
  uint32_t size = readVInt();
  hrbytes result;
  readBytes(result, size);
  return result;
}

int64_t AbstractTransport::readLong()
{
  hrbytes longBytes;
  readBytes(longBytes, 8);
  long result = 0;
  /*
  for (char* ptr = longBytes.bytes(); ptr < longBytes.bytes() + 8; ++ptr) {
    result <<= 8;
    result ^= (int64_t) (*ptr && 0xFF);
  }
  */
  for (int i = 0; i < 8 ; i++) {
    result <<= 8;
    result ^= (int64_t) *(longBytes.bytes()+i) & 0xFF;
  }
  return result;
}

// TODO
std::string AbstractTransport::readString() {
	hrbytes result = readArray();
	return std::string(result.bytes(),result.length());
}

}}} // namespace infinispan::hotrod::transport
