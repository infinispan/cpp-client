#ifndef ISPN_HOTROD_PROTOCOL_HOTRODCONSTANTS_H
#define ISPN_HOTROD_PROTOCOL_HOTRODCONSTANTS_H

#include "infinispan/hotrod/defs.h"

namespace infinispan {
namespace hotrod {
namespace protocol {

class HotRodConstants
{
  public:
    // hotrod protocol magic numbers
    static const uint8_t REQUEST_MAGIC  = 0xA0;
    static const uint8_t RESPONSE_MAGIC = 0xA1;

    // hotrod protocol version
    static const uint8_t VERSION_10 = 10;
    static const uint8_t VERSION_11 = 11;
    static const uint8_t VERSION_12 = 12;

    //hotrod consistenthash version
    static const uint8_t CONSISTENT_HASH_V2 = 2;

    // request operation code
    static const uint8_t PUT_REQUEST                    = 0x01;
    static const uint8_t GET_REQUEST                    = 0x03;
    static const uint8_t PUT_IF_ABSENT_REQUEST          = 0x05;
    static const uint8_t REPLACE_REQUEST                = 0x07;
    static const uint8_t REPLACE_IF_UNMODIFIED_REQUEST  = 0x09;
    static const uint8_t REMOVE_REQUEST                 = 0x0B;
    static const uint8_t REMOVE_IF_UNMODIFIED_REQUEST   = 0x0D;
    static const uint8_t CONTAINS_KEY_REQUEST           = 0x0F;
    static const uint8_t GET_WITH_VERSION_REQUEST       = 0x11;
    static const uint8_t CLEAR_REQUEST                  = 0x13;
    static const uint8_t STATS_REQUEST                  = 0x15;
    static const uint8_t PING_REQUEST                   = 0x17;
    static const uint8_t BULK_GET_REQUEST               = 0x19;
    static const uint8_t GET_WITH_METADATA_REQUEST      = 0x1B;
    static const uint8_t BULK_GET_KEYS_REQUEST          = 0x1D;
    // response operation code
    static const uint8_t PUT_RESPONSE                   = 0x02;
    static const uint8_t GET_RESPONSE                   = 0x04;
    static const uint8_t PUT_IF_ABSENT_RESPONSE         = 0x06;
    static const uint8_t REPLACE_RESPONSE               = 0x08;
    static const uint8_t REPLACE_IF_UNMODIFIED_RESPONSE = 0x0A;
    static const uint8_t REMOVE_RESPONSE                = 0x0C;
    static const uint8_t REMOVE_IF_UNMODIFIED_RESPONSE  = 0x0E;
    static const uint8_t CONTAINS_KEY_RESPONSE          = 0x10;
    static const uint8_t GET_WITH_VERSION_RESPONSE      = 0x12;
    static const uint8_t CLEAR_RESPONSE                 = 0x14;
    static const uint8_t STATS_RESPONSE                 = 0x16;
    static const uint8_t PING_RESPONSE                  = 0x18;
    static const uint8_t BULK_GET_RESPONSE              = 0x1A;
    static const uint8_t GET_WITH_METADATA_RESPONSE     = 0x1C;
    static const uint8_t BULK_GET_KEYS_RESPONSE         = 0x1E;
    // response error
    static const uint8_t ERROR_RESPONSE                 = 0x50;

    // response status
    static const uint8_t NO_ERROR_STATUS                    = 0x00;
    static const uint8_t INVALID_MAGIC_OR_MESSAGE_ID_STATUS = 0x81;
    static const uint8_t REQUEST_PARSING_ERROR_STATUS       = 0x84;
    static const uint8_t NOT_PUT_REMOVED_REPLACED_STATUS    = 0x01;
    static const uint8_t UNKNOWN_COMMAND_STATUS             = 0x82;
    static const uint8_t SERVER_ERROR_STATUS                = 0x85;
    static const uint8_t KEY_DOES_NOT_EXIST_STATUS          = 0x02;
    static const uint8_t UNKNOWN_VERSION_STATUS             = 0x83;
    static const uint8_t COMMAND_TIMEOUT_STATUS             = 0x86;

    static const uint8_t CLIENT_INTELLIGENCE_BASIC                   = 0x01;
    static const uint8_t CLIENT_INTELLIGENCE_TOPOLOGY_AWARE          = 0x02;
    static const uint8_t CLIENT_INTELLIGENCE_HASH_DISTRIBUTION_AWARE = 0x03;

    static const uint8_t INFINITE_LIFESPAN = 0x01;
    static const uint8_t INFINITE_MAXIDLE  = 0x02;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_HOTRODCONSTANTS_H
