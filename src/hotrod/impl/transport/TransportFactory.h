#ifndef ISPN_HOTROD_TRANSPORT_TRANSPORTFACTORY_H
#define ISPN_HOTROD_TRANSPORT_TRANSPORTFACTORY_H

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

#include "hotrod/impl/transport/Transport.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class TransportFactory
{
    public:
        // TODO: no singleton, one instance each RemoteCacheManager
        static TransportFactory& getTransportFactory();

        virtual Transport* getTransport() = 0;
        virtual Transport* getTransport(const hrbytes& key) = 0;
        virtual void releaseTransport(Transport* transport) = 0;
        virtual int getTransportCount() = 0;
        virtual void invalidateTransport(/*SocketAdress,*/Transport* transport) = 0;
        virtual ~TransportFactory() {}

    private:
        static TransportFactory* transportFactory;

    friend class RemoteCacheManager;
};

}}} // namespace

#endif  /* ISPN_HOTROD_TRANSPORT_TRANSPORTFACTORY_H */

