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

#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"

namespace infinispan {
namespace hotrod {

using protocol::Codec;
using namespace operations;

namespace transport {


TransportObjectFactory::TransportObjectFactory(
    Codec& c, TcpTransportFactory& factory, int64_t tid, bool pingable)
    : tcpTransportFactory(factory), topologyId(tid),
      pingOnStartup(pingable), codec(c), firstPingExecuted(false)
{ }

TcpTransport& TransportObjectFactory::makeObject(const InetSocketAddress& address) {
    TcpTransport* tcpTransport = new TcpTransport(address, tcpTransportFactory);
    if (pingOnStartup && !firstPingExecuted) {
        firstPingExecuted = true;

        // Don't ignore exceptions from ping() command, since
        // they indicate that the transport instance is invalid.
        ping(*tcpTransport, topologyId);
    }
    return *tcpTransport;
}

bool TransportObjectFactory::validateObject(const InetSocketAddress& /*address*/, TcpTransport& transport) {
    return ping(transport, topologyId) == SUCCESS;
}

void TransportObjectFactory::destroyObject(const InetSocketAddress& /*address*/, TcpTransport& transport) {
    delete &transport;
}

void TransportObjectFactory::activateObject(const InetSocketAddress& /*address*/, TcpTransport& /*transport*/) {
    // empty
}

void TransportObjectFactory::passivateObject(
    const InetSocketAddress& /*address*/, TcpTransport& /*transport*/)
{
  // empty
}

PingResult TransportObjectFactory::ping(
    TcpTransport& tcpTransport, int64_t _topologyId)
{
    PingOperation po(codec, _topologyId, tcpTransport);
    return po.execute();
}

}}} // namespace infinispan::hotrod::transport
