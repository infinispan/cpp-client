#ifndef ISPN_HOTROD_TRANSPORT_TRANSPORTOBJECTFACTORY_H
#define ISPN_HOTROD_TRANSPORT_TRANSPORTOBJECTFACTORY_H

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

#include "hotrod/impl/transport/tcp/KeyedPoolableObjectFactory.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/operations/PingOperation.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class TcpTransportFactory;

class TransportObjectFactory
  : public KeyedPoolableObjectFactory<InetSocketAddress, TcpTransport>
{
  public:
    TransportObjectFactory(
        protocol::Codec& codec,
        TcpTransportFactory& tcpTransportFactory,
        int64_t topologyId, bool pingOnStartup);

    TcpTransport& makeObject(const InetSocketAddress& address);
    bool validateObject(const InetSocketAddress& address, TcpTransport& transport);
    void destroyObject(const InetSocketAddress& address, TcpTransport& transport);

    void activateObject(const InetSocketAddress& address, TcpTransport& transport);
    void passivateObject(const InetSocketAddress& address, TcpTransport& transport);

  private:
    TcpTransportFactory& tcpTransportFactory;
    int64_t topologyId;
    bool pingOnStartup;
    infinispan::hotrod::protocol::Codec& codec;

    bool firstPingExecuted;

    operations::PingResult ping(TcpTransport& tcpTransport, int64_t topologyId);
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_TRANSPORTOBJECTFACTORY_H */
