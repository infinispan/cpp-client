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

#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"


namespace infinispan {
namespace hotrod {
namespace transport {

// TODO

Transport* TcpTransportFactory::getTransport()
{
    return new TcpTransport();
}

Transport* TcpTransportFactory::getTransport(const hrbytes& key)
{
    Transport* transport = 0;
    hrbytes key_ = const_cast<hrbytes&>(key);
    if (key_.length() < 1000) {
    	transport = new TcpTransport();
    }
    return transport;
}

void TcpTransportFactory::releaseTransport(Transport* transport)
{
    delete transport;
}

int TcpTransportFactory::getTransportCount()
{
    return 0;
}

void TcpTransportFactory::invalidateTransport(/*SocketAdress,*/Transport* transport)
{
    delete transport;
}

}}} /* namespace */
