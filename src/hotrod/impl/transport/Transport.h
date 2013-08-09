#ifndef ISPN_HOTROD_TRANSPORT_TRANSPORT_H
#define ISPN_HOTROD_TRANSPORT_TRANSPORT_H

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

#include <stdint.h>

namespace infinispan {
namespace hotrod {
namespace transport {

class Transport
{
  public:
    virtual void flush() = 0;
    virtual void writeByte(uint8_t uchar) = 0;
    virtual void writeVInt(uint32_t uint) = 0;
    virtual void writeVLong(uint64_t ulong) = 0;
    virtual void writeArray(const hrbytes& bytes) = 0;

    virtual void writeLong(int64_t slong) = 0;

    virtual uint8_t readByte() = 0;
    virtual uint32_t readVInt() = 0;
    virtual uint64_t readVLong() = 0;
    virtual hrbytes readArray() = 0;

    virtual int64_t readLong() = 0;
    virtual std::string readString() = 0;

    virtual void release() = 0;
    virtual void invalidate() = 0;

    virtual ~Transport() {}
};

}}} // namespace infinispan::hotrod::transport

#endif  // ISPN_HOTROD_TRANSPORT_TRANSPORT_H

