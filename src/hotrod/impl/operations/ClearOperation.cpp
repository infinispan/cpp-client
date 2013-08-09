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
#include "hotrod/impl/operations/ClearOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

ClearOperation::ClearOperation(
    const Codec&      codec_,
    infinispan::hotrod::transport::TransportFactory* transportFactory_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    uint32_t    flags_)
    : RetryOnFailureOperation<hrbytes>(
        codec_, transportFactory_, cacheName_, topologyId_, flags_)
{}

Transport& ClearOperation::getTransport(int /*retryCount*/)
{
        return RetryOnFailureOperation<hrbytes>::transportFactory->getTransport();
}

hrbytes ClearOperation::executeOperation(infinispan::hotrod::transport::Transport& transport)
{
    hr_scoped_ptr<HeaderParams> params(&(RetryOnFailureOperation<hrbytes>::writeHeader(transport, CLEAR_REQUEST)));
    transport.flush();
    RetryOnFailureOperation<hrbytes>::readHeaderAndValidate(transport, *params);
    return hrbytes();
}

}}} /// namespace infinispan::hotrod::operations
