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
#include "hotrod/impl/operations/BulkGetOperation.h"

#include <map>

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

BulkGetOperation::BulkGetOperation(
    const Codec&      codec_,
    infinispan::hotrod::transport::TransportFactory* transportFactory_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    uint32_t    flags_,
    int32_t  entryCount_)
    : RetryOnFailureOperation<std::map<hrbytes, hrbytes> >(
        codec_, transportFactory_, cacheName_, topologyId_, flags_), entryCount(entryCount_)
{}

Transport& BulkGetOperation::getTransport(int /*retryCount*/)
{
        return RetryOnFailureOperation<std::map<hrbytes, hrbytes> >::transportFactory->getTransport();
}

std::map<hrbytes,hrbytes> BulkGetOperation::executeOperation(infinispan::hotrod::transport::Transport& transport)
{
    hr_scoped_ptr<HeaderParams> params(&(RetryOnFailureOperation<std::map<hrbytes, hrbytes> >::writeHeader(transport, BULK_GET_REQUEST)));
    transport.writeVInt(entryCount);
    transport.flush();
    RetryOnFailureOperation<std::map<hrbytes, hrbytes> >::readHeaderAndValidate(transport, *params);
    std::map<hrbytes,hrbytes> result;
    while (transport.readByte()==1) {
        hrbytes key = transport.readArray();
        hrbytes value = transport.readArray();
        result[key] = value;
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations
