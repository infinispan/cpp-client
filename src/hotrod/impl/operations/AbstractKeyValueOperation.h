#ifndef ISPN_HOTROD_OPERATIONS_ABSTRACTKEYVALUEOPERATION_H
#define ISPN_HOTROD_OPERATIONS_ABSTRACTKEYVALUEOPERATION_H

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

#include "hotrod/impl/operations/AbstractKeyOperation.h"

#include <set>

namespace infinispan {
namespace hotrod {
namespace operations {

template<class T> class AbstractKeyValueOperation : public AbstractKeyOperation<T>
{
    protected:
	    AbstractKeyValueOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            infinispan::hotrod::transport::TransportFactory& transportFactory_,
            const hrbytes&                                   key_,
            const hrbytes&                                   cacheName_,
            uint32_t                                         topologyId_,
            const std::set<Flag>&                                   flags_,
            const hrbytes&                                   value_,
            uint32_t                                         lifespan_,
            uint32_t                                         maxIdle_)
            : AbstractKeyOperation<T>(codec_, transportFactory_, key_,
                                     cacheName_,
                                      topologyId_, flags_),
            value(value_), lifespan(lifespan_), maxIdle(maxIdle_)
        {}

        const hrbytes& value;
        uint32_t lifespan;
        uint32_t maxIdle;

        //[header][key length][key][lifespan][max idle][value length][value]
        uint8_t sendPutOperation(
            infinispan::hotrod::transport::Transport&     transport,
            uint8_t                                       opCode,
            uint8_t                                       /*opRespCode*/)
        {
            // 1) write header
            hr_scoped_ptr<infinispan::hotrod::protocol::HeaderParams> params(&(AbstractKeyOperation<T>::writeHeader(transport, opCode)));

            // 2) write key and value
            transport.writeArray(AbstractKeyOperation<T>::key);
            transport.writeVInt(lifespan);
            transport.writeVInt(maxIdle);
            transport.writeArray(value);
            transport.flush();

            // 3) now read header

            //return status (not error status for sure)
            return AbstractKeyOperation<T>::readHeaderAndValidate(transport, *params);
        }
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_ABSTRACTKEYVALUEOPERATION_H
