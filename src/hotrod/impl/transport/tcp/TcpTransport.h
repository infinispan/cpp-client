#ifndef ISPN_HOTROD_TCPTRANSPORT_H
#define ISPN_HOTROD_TCPTRANSPORT_H

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

#include <sstream>

#include "hotrod/impl/transport/AbstractTransport.h"

#include "hotrod/sys/types.h"
#include "hotrod/impl/transport/tcp/Socket.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class TransportFactory;

class TcpTransport : public AbstractTransport
{
  public:
    TcpTransport(const InetSocketAddress& address, TransportFactory& factory);

    void flush();
    void writeByte(uint8_t uchar);
    void writeVInt(uint32_t uint);
    void writeVLong(uint64_t ulong);
    void writeBytes(const hrbytes& bytes);

    uint8_t readByte();
    uint32_t readVInt();
    uint64_t readVLong();
    void readBytes(hrbytes& bytes, uint32_t size);

    void release();
    void invalidate();
    void destroy();

    const InetSocketAddress& getServerAddress();

  private:
    TcpTransport();
    Socket socket;
    bool invalid;
    const InetSocketAddress& serverAddress;

    bool isValid();

  friend class TcpTransportFactory;
};

}}} // namespace infinispan::hotrod::transport::tcp

#endif  /* ISPN_HOTROD_TCPTRANSPORT_H */

