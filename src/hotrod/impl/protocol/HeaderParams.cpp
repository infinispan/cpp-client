

#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "infinispan/hotrod/exceptions.h"

#include <sstream>

namespace infinispan {
namespace hotrod {
// Well known media types and their ids mapping
std::map<std::string, int> IdsMediaTypes::typesIds =
{
        std::pair<std::string,int>(APPLICATION_OBJECT_TYPE, 1),
        std::pair<std::string,int>(APPLICATION_JSON_TYPE, 2),
        std::pair<std::string,int>(APPLICATION_OCTET_STREAM_TYPE, 3),
        std::pair<std::string,int>(APPLICATION_PDF_TYPE, 4),
        std::pair<std::string,int>(APPLICATION_RTF_TYPE, 5),
        std::pair<std::string,int>(APPLICATION_ZIP_TYPE, 6),
        std::pair<std::string,int>(IMAGE_GIF_TYPE, 7),
        std::pair<std::string,int>(IMAGE_JPEG_TYPE, 8),
        std::pair<std::string,int>(IMAGE_PNG_TYPE, 9),
        std::pair<std::string,int>(TEXT_CSS_TYPE, 10),
        std::pair<std::string,int>(TEXT_CSV_TYPE, 11),
        std::pair<std::string,int>(APPLICATION_PROTOSTREAM_TYPE, 12),
        std::pair<std::string,int>(TEXT_PLAIN_TYPE, 13),
        std::pair<std::string,int>(TEXT_HTML_TYPE, 14),
        std::pair<std::string,int>(APPLICATION_JBOSS_MARSHALLING_TYPE, 15),
        std::pair<std::string,int>(APPLICATION_INFINISPAN_MARSHALLING_TYPE, 16)
};



MediaType::MediaType(const char* typeSubtype) {
    if (typeSubtype != nullptr) {
        this->typeSubtype = typeSubtype;
        auto pos = this->typeSubtype.find('/');
        if (pos != std::string::npos) {
            this->type = this->typeSubtype.substr(0,pos);
            this->subType = this->typeSubtype.substr(pos, this->typeSubtype.length());
        }
    }
}

int IdsMediaTypes::getByMediaType(std::string mediaType) {
    if (typesIds.find(mediaType) != typesIds.end())
    {
        return typesIds[mediaType];
    }
    return -1;
}

std::string IdsMediaTypes::getById(int id) {
    for (auto it = typesIds.begin(); it != typesIds.end(); ++it)
    {
       if (it->second == id)
       {
           return it->first;
       }
    }
    return std::string();
}

namespace protocol {
HeaderParams::HeaderParams(Topology& tid):topologyId(tid) {
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
    case HotRodConstants::QUERY_REQUEST:
        return HotRodConstants::QUERY_RESPONSE;
    case HotRodConstants::AUTH_MECH_LIST_REQUEST:
        return HotRodConstants::AUTH_MECH_LIST_RESPONSE;
    case HotRodConstants::AUTH_REQUEST:
        return HotRodConstants::AUTH_RESPONSE;
    case HotRodConstants::ADD_CLIENT_LISTENER_REQUEST:
        return HotRodConstants::ADD_CLIENT_LISTENER_RESPONSE;
    case HotRodConstants::REMOVE_CLIENT_LISTENER_REQUEST:
        return HotRodConstants::REMOVE_CLIENT_LISTENER_RESPONSE;
    case HotRodConstants::GET_ALL_REQUEST:
        return HotRodConstants::GET_ALL_RESPONSE;
    case HotRodConstants::ITERATION_START_REQUEST:
       return HotRodConstants::ITERATION_START_RESPONSE;
    case HotRodConstants::ITERATION_NEXT_REQUEST:
       return HotRodConstants::ITERATION_NEXT_RESPONSE;
    case HotRodConstants::ITERATION_END_REQUEST:
       return HotRodConstants::ITERATION_END_RESPONSE;
    case HotRodConstants::GET_STREAM_REQUEST:
       return HotRodConstants::GET_STREAM_RESPONSE;
    case HotRodConstants::PUT_STREAM_REQUEST:
       return HotRodConstants::PUT_STREAM_RESPONSE;
    case HotRodConstants::COUNTER_CREATE_REQUEST:
       return HotRodConstants::COUNTER_CREATE_RESPONSE;
    case HotRodConstants::COUNTER_GET_CONFIGURATION_REQUEST:
       return HotRodConstants::COUNTER_GET_CONFIGURATION_RESPONSE;
    case HotRodConstants::COUNTER_IS_DEFINED_REQUEST:
       return HotRodConstants::COUNTER_IS_DEFINED_RESPONSE;
    case HotRodConstants::COUNTER_REMOVE_REQUEST:
       return HotRodConstants::COUNTER_REMOVE_RESPONSE;
    case HotRodConstants::COUNTER_ADD_AND_GET_REQUEST:
       return HotRodConstants::COUNTER_ADD_AND_GET_RESPONSE;
    case HotRodConstants::COUNTER_GET_REQUEST:
       return HotRodConstants::COUNTER_GET_RESPONSE;
    case HotRodConstants::COUNTER_RESET_REQUEST:
       return HotRodConstants::COUNTER_RESET_RESPONSE;
    case HotRodConstants::COUNTER_CAS_REQUEST:
       return HotRodConstants::COUNTER_CAS_RESPONSE;
    case HotRodConstants::COUNTER_GET_NAMES_REQUEST:
       return HotRodConstants::COUNTER_GET_NAMES_RESPONSE;
    case HotRodConstants::COUNTER_ADD_LISTENER_REQUEST:
       return HotRodConstants::COUNTER_ADD_LISTENER_RESPONSE;
    case HotRodConstants::COUNTER_REMOVE_LISTENER_REQUEST:
       return HotRodConstants::COUNTER_REMOVE_LISTENER_RESPONSE;
    case HotRodConstants::PREPARE_REQUEST:
        return HotRodConstants::PREPARE_RESPONSE;
    case HotRodConstants::COMMIT_REQUEST:
        return HotRodConstants::COMMIT_RESPONSE;
    case HotRodConstants::ROLLBACK_REQUEST:
        return HotRodConstants::ROLLBACK_RESPONSE;
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

HeaderParams& HeaderParams::setDataFormat(DataFormat* df) {
	dataFormat = df;
	return *this;
}

uint64_t HeaderParams::getMessageId()
{
	return messageId;
}
}}} // namespace
