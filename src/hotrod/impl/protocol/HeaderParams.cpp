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

#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/protocol/HotRodConstants.h"

#include <sstream>

namespace infinispan {
namespace hotrod {
namespace protocol {


HeaderParams& HeaderParams::setOpCode(uint8_t code) {
	opCode = code;
    opRespCode = toOpRespCode(code);
    return *this;
}

HeaderParams& HeaderParams::setCacheName(const hrbytes& c) {
    cacheName = c;
    return *this;
}

HeaderParams& HeaderParams::setFlags(uint32_t vec) {
    flags = vec;
    return *this;
}

HeaderParams& HeaderParams::setClientIntel(uint8_t intel) {
    clientIntel = intel;
    return *this;
}

HeaderParams& HeaderParams::setTxMarker(uint8_t marker) {
    txMarker = marker;
    return *this;
}

HeaderParams& HeaderParams::setTopologyId(uint32_t id) {
    topologyId = id;
    return *this;
}

HeaderParams& HeaderParams::setMessageId(uint64_t id) {
    messageId = id;
    return *this;
}

uint8_t HeaderParams::toOpRespCode(uint8_t code) {
    switch(code) {
    case HotRodConstants::PUT_REQUEST:
        return HotRodConstants::PUT_RESPONSE;
    case HotRodConstants::GET_REQUEST:
        return HotRodConstants::GET_RESPONSE;
    case HotRodConstants::PUT_IF_ABSENT_REQUEST:
        return HotRodConstants::PUT_IF_ABSENT_RESPONSE;
    case HotRodConstants::REPLACE_REQUEST:
        return HotRodConstants::REPLACE_RESPONSE;
    case HotRodConstants::REPLACE_IF_UNMODIFIED_REQUEST:
        return HotRodConstants::REPLACE_IF_UNMODIFIED_RESPONSE;
    case HotRodConstants::REMOVE_REQUEST:
        return HotRodConstants::REMOVE_RESPONSE;
    case HotRodConstants::REMOVE_IF_UNMODIFIED_REQUEST:
        return HotRodConstants::REMOVE_IF_UNMODIFIED_RESPONSE;
    case HotRodConstants::CONTAINS_KEY_REQUEST:
        return HotRodConstants::CONTAINS_KEY_RESPONSE;
    case HotRodConstants::GET_WITH_VERSION_REQUEST:
        return HotRodConstants::GET_WITH_VERSION_RESPONSE;
    case HotRodConstants::CLEAR_REQUEST:
        return HotRodConstants::CLEAR_RESPONSE;
    case HotRodConstants::STATS_REQUEST:
        return HotRodConstants::STATS_RESPONSE;
    case HotRodConstants::PING_REQUEST:
        return HotRodConstants::PING_RESPONSE;
    case HotRodConstants::BULK_GET_REQUEST:
        return HotRodConstants::BULK_GET_RESPONSE;
    case HotRodConstants::GET_WITH_METADATA_REQUEST:
        return HotRodConstants::GET_WITH_METADATA_RESPONSE;
    case HotRodConstants::BULK_GET_KEYS_REQUEST:
        return HotRodConstants::BULK_GET_KEYS_RESPONSE;
    default:
    	std::ostringstream msg;
    	msg << "Unknown operation code: " << opCode;
        throw InternalException(msg.str());
    }
}

}}} // namespace
