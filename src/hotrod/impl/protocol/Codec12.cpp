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

#include "hotrod/impl/protocol/Codec12.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/transport/Transport.h"

#include <iostream>
#include <sstream>

namespace infinispan {
namespace hotrod {

using transport::Transport;

namespace protocol {

// TODO : multithread management
long Codec12::msgId = 0;

HeaderParams& Codec12::writeHeader(
    Transport& transport, HeaderParams& params) const
{
    return writeHeader(transport, params, HotRodConstants::VERSION_12);
}

HeaderParams& Codec12::writeHeader(
    Transport& transport, HeaderParams& params, uint8_t version) const
{
    transport.writeByte(HotRodConstants::REQUEST_MAGIC);
    transport.writeVLong(params.setMessageId(++msgId).messageId);
    transport.writeByte(version);
    transport.writeByte(params.opCode);
    transport.writeArray(params.cacheName);
    int32_t flagInt = 0;
    for (std::set<Flag>::const_iterator iter=params.flags.begin();
         iter!=params.flags.end(); ++iter)
    {
        flagInt |= *iter;
    }
    transport.writeVInt(flagInt);
    transport.writeByte(params.clientIntel);
    transport.writeVInt(params.topologyId);
    transport.writeByte(params.txMarker);
    return params;
}

uint8_t Codec12::readHeader(
    Transport& transport, const HeaderParams& params) const
{
    uint8_t magic = transport.readByte();
    if (magic != HotRodConstants::RESPONSE_MAGIC) {
        std::ostringstream message;
        message << "Invalid magic number. Expected " <<
            HotRodConstants::RESPONSE_MAGIC << " and received " << magic;
        throw InvalidResponseException(message.str());
    }

    uint64_t receivedMessageId = transport.readVLong();
    // TODO: java comment, to be checked
    // If received id is 0, it could be that a failure was noted before the
    // message id was detected, so don't consider it to a message id error
    if (receivedMessageId != params.messageId && receivedMessageId != 0) {
        std::ostringstream message;
        message << "Invalid message id. Expected " <<
            params.messageId << " and received " << receivedMessageId;
        throw InvalidResponseException(message.str());
    }

    uint8_t receivedOpCode = transport.readByte();

    // Read both the status and new topology (if present),
    // before deciding how to react to error situations.
    uint8_t status = transport.readByte();
    readNewTopologyIfPresent(transport, params);

    // Now that all headers values have been read, check the error responses.
    // This avoids situations where an exceptional return ends up with
    // the socket containing data from previous request responses.
    if (receivedOpCode != params.opRespCode) {
    	if (receivedOpCode == HotRodConstants::ERROR_RESPONSE) {
    		checkForErrorsInResponseStatus(transport, params, status);
        }
        std::ostringstream message;
        message << "Invalid response operation. Expected " <<
            params.opRespCode << " and received " << receivedOpCode;
        throw InvalidResponseException(message.str());
    }

    return status;
}

void Codec12::readNewTopologyIfPresent(
    Transport& transport, const HeaderParams& params) const
{
    uint8_t topologyChangeByte = transport.readByte();
    if (topologyChangeByte == 1)
        readNewTopologyAndHash(transport, params.topologyId);
}

void Codec12::readNewTopologyAndHash(
    Transport& /*transport*/, uint32_t /*topologyId*/) const
{
    // TODO
    return;
}

void Codec12::checkForErrorsInResponseStatus(
    Transport& /*transport*/, const HeaderParams& /*params*/, uint8_t /*status*/) const
{
    // TODO
    return;
}

}}} // namespace infinispan::hotrod::protocol
