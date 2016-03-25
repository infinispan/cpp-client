

#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "infinispan/hotrod/exceptions.h"

#include <sstream>

namespace infinispan {
namespace hotrod {
namespace protocol {

HeaderParams::HeaderParams(Topology& tid):topologyId(tid){
}

HeaderParams& HeaderParams::setOpCode(uint8_t code) {
	opCode = code;
    opRespCode = toOpRespCode(code);
    return *this;
}

HeaderParams& HeaderParams::setCacheName(const std::vector<char>& c) {
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
    case HotRodConstants::SIZE_REQUEST:
        return HotRodConstants::SIZE_RESPONSE;
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
    case HotRodConstants::EXEC_REQUEST:
        return HotRodConstants::EXEC_RESPONSE;
    default:
    	std::ostringstream msg;
    	msg << "Unknown operation code: " << opCode;
        throw InternalException(msg.str());
    }
}

HeaderParams& HeaderParams::setTopologyAge(int topologyAge_) {
	topologyAge=topologyAge_;
	return *this;
}

}}} // namespace
