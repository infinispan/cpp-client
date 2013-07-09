#ifndef ISPN_HOTROD_OPERATIONS_RETRYONFAILUREOPERATION_H
#define ISPN_HOTROD_OPERATIONS_RETRYONFAILUREOPERATION_H

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

#include "hotrod/impl/operations/HotRodOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"

namespace infinispan {
namespace hotrod {
namespace operations {

template<class T> class RetryOnFailureOperation : public HotRodOperation<T>
{
    public:
        T execute()
        {
            // TODO : retry on failure management
        	int retryCount = 0;
        	infinispan::hotrod::transport::Transport* transport = getTransport(retryCount);
            T result = executeOperation(transport);
            transportFactory.releaseTransport(transport);
            return result;
        }

    protected:
        RetryOnFailureOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            infinispan::hotrod::transport::TransportFactory& transportFactory_,
            const hrbytes&                                         cacheName_,
            uint32_t                                         topologyId_,
            const std::set<Flag>&                                   flags_)
            : HotRodOperation<T>(codec_, flags_, cacheName_, topologyId_),
              transportFactory(transportFactory_)
        {}

        infinispan::hotrod::transport::TransportFactory& transportFactory;

        virtual infinispan::hotrod::transport::Transport* getTransport(int retryCount) = 0;
        virtual T executeOperation(
            infinispan::hotrod::transport::Transport* transport) = 0;

        virtual ~RetryOnFailureOperation() {}
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_RETRYONFAILUREOPERATION_H
