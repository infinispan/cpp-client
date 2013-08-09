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

#include "hotrod/impl/operations/ReplaceIfUnmodifiedOperation.h"
#include "hotrod/sys/types.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

ReplaceIfUnmodifiedOperation::ReplaceIfUnmodifiedOperation(
    const Codec& _codec, TransportFactory* _transportFactory, const hrbytes& _key,
    const hrbytes& _cacheName, uint32_t _topologyId, uint32_t _flags,
    const hrbytes& _value, uint32_t _lifespan, uint32_t _maxIdle, int64_t _version) :
        AbstractKeyValueOperation<VersionedOperationResponse>(
            _codec, _transportFactory, _key, _cacheName, _topologyId,
            _flags, _value, _lifespan, _maxIdle), version(_version)
{}

VersionedOperationResponse ReplaceIfUnmodifiedOperation::executeOperation(
    Transport& transport)
{
    // 1) write header
    hr_scoped_ptr<infinispan::hotrod::protocol::HeaderParams> params(
        &(AbstractKeyOperation<VersionedOperationResponse>::writeHeader(
            transport, REPLACE_IF_UNMODIFIED_REQUEST)));

    //2) write message body
    transport.writeArray(key);
    transport.writeVInt(lifespan);
    transport.writeVInt(maxIdle);
    transport.writeLong(version);
    transport.writeArray(value);
    transport.flush();

    return AbstractKeyValueOperation<VersionedOperationResponse>::returnVersionedOperationResponse(transport, *params);
}


}}} // namespace infinispan::hotrod::operations
