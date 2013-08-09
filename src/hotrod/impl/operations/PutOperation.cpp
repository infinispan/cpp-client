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
#include "hotrod/impl/operations/PutOperation.h"

#include <sstream>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

PutOperation::PutOperation(
    const Codec&      codec_,
    TransportFactory* transportFactory_,
    const hrbytes&    key_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    uint32_t flags_,
    const hrbytes&    value_,
    uint32_t          lifespan_,
    uint32_t          maxIdle_)
    : AbstractKeyValueOperation<hrbytes>(codec_, transportFactory_, key_,
        cacheName_, topologyId_, flags_, value_, lifespan_, maxIdle_)
{}

hrbytes PutOperation::executeOperation(Transport& transport) {
    uint8_t status = sendPutOperation(transport, PUT_REQUEST, PUT_RESPONSE);
   if (status != NO_ERROR_STATUS) {
        std::ostringstream message;
        message << "Unexpected response status: " << status;
        throw InvalidResponseException(message.str());
    }
    return AbstractKeyValueOperation<hrbytes>::returnPossiblePrevValue(transport);
}


}}} /* namespace */
