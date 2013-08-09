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
#include "hotrod/impl/operations/GetWithMetadataOperation.h"
#include "hotrod/impl/MetadataValueImpl.h"
#include <cstring>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

GetWithMetadataOperation::GetWithMetadataOperation(
    const Codec&      codec_,
    TransportFactory* transportFactory_,
    const hrbytes&    key_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    uint32_t    flags_)
    : AbstractKeyOperation<MetadataValueImpl<hrbytes> >(
        codec_, transportFactory_, key_, cacheName_, topologyId_, flags_)
{}

MetadataValueImpl<hrbytes> GetWithMetadataOperation::executeOperation(Transport& transport)
{
    MetadataValueImpl<hrbytes> result;
    uint8_t status = sendKeyOperation(
        key, transport, GET_WITH_METADATA_REQUEST, GET_WITH_METADATA_RESPONSE);
    if (status == NO_ERROR_STATUS) {
        uint8_t flag = transport.readByte();
        if ((flag & INFINITE_LIFESPAN) != INFINITE_LIFESPAN) {
        	result.setCreated(transport.readLong());
        	result.setLifespan(transport.readVInt());
        }
        if ((flag & INFINITE_MAXIDLE) != INFINITE_MAXIDLE) {
            result.setLastUsed(transport.readLong());
            result.setMaxIdle(transport.readVInt());
        }
        result.setVersion(transport.readLong());
        result.setValue(transport.readArray());
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations
