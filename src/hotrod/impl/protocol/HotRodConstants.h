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
    static const uint8_t VERSION_13 = 13;
    static const uint8_t VERSION_20 = 20;
    static const uint8_t VERSION_21 = 21;
    static const uint8_t VERSION_22 = 22;
    static const uint8_t VERSION_23 = 23;
    static const uint8_t VERSION_24 = 24;
    static const uint8_t VERSION_25 = 25;
    static const uint8_t VERSION_26 = 26;
    static const uint8_t VERSION_27 = 27;

    //hotrod consistenthash version
    static const uint8_t CONSISTENT_HASH_V2      = 2;
    static const uint8_t SEGMENT_CONSISTENT_HASH = 3;

    //requests
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
    static const uint8_t QUERY_REQUEST                  = 0x1F;
    static const uint8_t AUTH_MECH_LIST_REQUEST         = 0x21;
    static const uint8_t AUTH_REQUEST                   = 0x23;
    static const uint8_t ADD_CLIENT_LISTENER_REQUEST    = 0x25;
    static const uint8_t REMOVE_CLIENT_LISTENER_REQUEST = 0x27;
    static const uint8_t SIZE_REQUEST                   = 0x29;
    static const uint8_t EXEC_REQUEST                   = 0x2B;
    static const uint8_t PUT_ALL_REQUEST                = 0x2D;
    static const uint8_t GET_ALL_REQUEST                = 0x2F;
    static const uint8_t ITERATION_START_REQUEST        = 0x31;
    static const uint8_t ITERATION_NEXT_REQUEST         = 0x33;
    static const uint8_t ITERATION_END_REQUEST          = 0x35;
    static const uint8_t GET_STREAM_REQUEST             = 0x37;
    static const uint8_t PUT_STREAM_REQUEST             = 0x39;
    static const uint8_t PREPARE_REQUEST                = 0x3B;
    static const uint8_t COMMIT_REQUEST                 = 0x3D;
    static const uint8_t ROLLBACK_REQUEST               = 0x3F;
    static const uint8_t COUNTER_CREATE_REQUEST         = 0x4B;
    static const uint8_t COUNTER_GET_CONFIGURATION_REQUEST = 0x4D;
    static const uint8_t COUNTER_IS_DEFINED_REQUEST     = 0x4F;
    static const uint8_t COUNTER_ADD_AND_GET_REQUEST    = 0x52;
    static const uint8_t COUNTER_RESET_REQUEST          = 0x54;
    static const uint8_t COUNTER_GET_REQUEST            = 0x56;
    static const uint8_t COUNTER_CAS_REQUEST            = 0x58;
    static const uint8_t COUNTER_ADD_LISTENER_REQUEST   = 0x5A;
    static const uint8_t COUNTER_REMOVE_LISTENER_REQUEST = 0x5C;
    static const uint8_t COUNTER_REMOVE_REQUEST         = 0x5E;
    static const uint8_t COUNTER_GET_NAMES_REQUEST      = 0x64;
    //responses
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
    static const uint8_t QUERY_RESPONSE                 = 0x20;
    static const uint8_t AUTH_MECH_LIST_RESPONSE        = 0x22;
    static const uint8_t AUTH_RESPONSE                  = 0x24;
    static const uint8_t ADD_CLIENT_LISTENER_RESPONSE   = 0x26;
    static const uint8_t REMOVE_CLIENT_LISTENER_RESPONSE = 0x28;
    static const uint8_t SIZE_RESPONSE                  = 0x2A;
    static const uint8_t EXEC_RESPONSE                  = 0x2C;
    static const uint8_t PUT_ALL_RESPONSE               = 0x2E;
    static const uint8_t GET_ALL_RESPONSE               = 0x30;
    static const uint8_t ITERATION_NEXT_RESPONSE        = 0x34;
    static const uint8_t ITERATION_END_RESPONSE         = 0x36;
    static const uint8_t ITERATION_START_RESPONSE       = 0x32;
    static const uint8_t GET_STREAM_RESPONSE            = 0x38;
    static const uint8_t PUT_STREAM_RESPONSE            = 0x3A;
    static const uint8_t PREPARE_RESPONSE               = 0x3C;
    static const uint8_t COMMIT_RESPONSE                = 0x3E;
    static const uint8_t ROLLBACK_RESPONSE              = 0x40;
    static const uint8_t ERROR_RESPONSE                 = 0x50;
    static const uint8_t CACHE_ENTRY_CREATED_EVENT_RESPONSE = 0x60;
    static const uint8_t CACHE_ENTRY_MODIFIED_EVENT_RESPONSE = 0x61;
    static const uint8_t CACHE_ENTRY_REMOVED_EVENT_RESPONSE = 0x62;
    static const uint8_t CACHE_ENTRY_EXPIRED_EVENT_RESPONSE = 0x63;
    static const uint8_t COUNTER_CREATE_RESPONSE            = 0x4C;
    static const uint8_t COUNTER_GET_CONFIGURATION_RESPONSE = 0x4E;
    static const uint8_t COUNTER_IS_DEFINED_RESPONSE        = 0x51;
    static const uint8_t COUNTER_ADD_AND_GET_RESPONSE       = 0x53;
    static const uint8_t COUNTER_RESET_RESPONSE             = 0x55;
    static const uint8_t COUNTER_GET_RESPONSE               = 0x57;
    static const uint8_t COUNTER_CAS_RESPONSE               = 0x59;
    static const uint8_t COUNTER_ADD_LISTENER_RESPONSE      = 0x5B;
    static const uint8_t COUNTER_REMOVE_LISTENER_RESPONSE   = 0x5D;
    static const uint8_t COUNTER_REMOVE_RESPONSE            = 0x5F;
    static const uint8_t COUNTER_GET_NAMES_RESPONSE         = 0x65;
    static const uint8_t COUNTER_EVENT_RESPONSE             = 0x66;

    //response status
    static const uint8_t NO_ERROR_STATUS                    = 0x00;
    static const uint8_t NOT_PUT_REMOVED_REPLACED_STATUS = 0x01;
    static const uint8_t KEY_DOES_NOT_EXIST_STATUS = 0x02;
    static const uint8_t SUCCESS_WITH_PREVIOUS = 0x03;
    static const uint8_t NOT_EXECUTED_WITH_PREVIOUS = 0x04;
    static const uint8_t INVALID_ITERATION = 0x05;
    static const uint8_t NO_ERROR_STATUS_COMPAT = 0x06;
    static const uint8_t SUCCESS_WITH_PREVIOUS_COMPAT = 0x07;
    static const uint8_t NOT_EXECUTED_WITH_PREVIOUS_COMPAT = 0x08;

    static const uint8_t INVALID_MAGIC_OR_MESSAGE_ID_STATUS = 0x81;
    static const uint8_t REQUEST_PARSING_ERROR_STATUS       = 0x84;
    static const uint8_t UNKNOWN_COMMAND_STATUS             = 0x82;
    static const uint8_t SERVER_ERROR_STATUS                = 0x85;
    static const uint8_t UNKNOWN_VERSION_STATUS             = 0x83;
    static const uint8_t COMMAND_TIMEOUT_STATUS             = 0x86;
    static const uint8_t NODE_SUSPECTED                     = 0x87;
    static const uint8_t ILLEGAL_LIFECYCLE_STATE            = 0x88;

    static const uint8_t CLIENT_INTELLIGENCE_BASIC                   = 0x01;
    static const uint8_t CLIENT_INTELLIGENCE_TOPOLOGY_AWARE          = 0x02;
    static const uint8_t CLIENT_INTELLIGENCE_HASH_DISTRIBUTION_AWARE = 0x03;

    static const uint8_t INFINITE_LIFESPAN = 0x01;
    static const uint8_t INFINITE_MAXIDLE  = 0x02;

    static const int8_t NO_TOPOLOGY             =  0;
    static const int8_t DEFAULT_CACHE_TOPOLOGY  = -1;
    static const int8_t SWITCH_CLUSTER_TOPOLOGY = -2;


    static bool isSuccess(short status) {
       return status == NO_ERROR_STATUS
          || status == NO_ERROR_STATUS_COMPAT
          || status == SUCCESS_WITH_PREVIOUS
          || status == SUCCESS_WITH_PREVIOUS_COMPAT;
    }

    static bool isNotExecuted(short status) {
       return status == NOT_PUT_REMOVED_REPLACED_STATUS
          || status == NOT_EXECUTED_WITH_PREVIOUS
          || status == NOT_EXECUTED_WITH_PREVIOUS_COMPAT;
    }

    static bool isNotExist(short status) {
       return status == KEY_DOES_NOT_EXIST_STATUS;
    }

    static bool hasPrevious(short status) {
       return status == SUCCESS_WITH_PREVIOUS
          || status == SUCCESS_WITH_PREVIOUS_COMPAT
          || status == NOT_EXECUTED_WITH_PREVIOUS
          || status == NOT_EXECUTED_WITH_PREVIOUS_COMPAT;
    }

    static bool hasCompatibility(short status) {
       return status == NO_ERROR_STATUS_COMPAT
          || status == SUCCESS_WITH_PREVIOUS_COMPAT
          || status == NOT_EXECUTED_WITH_PREVIOUS_COMPAT;
    }

    static bool isInvalidIteration(short status) {
       return status == INVALID_ITERATION;
    }
    static bool isEvent(short opCode) {
    	return opCode ==  CACHE_ENTRY_CREATED_EVENT_RESPONSE || opCode == CACHE_ENTRY_EXPIRED_EVENT_RESPONSE
    	        			|| opCode == CACHE_ENTRY_MODIFIED_EVENT_RESPONSE || opCode == CACHE_ENTRY_REMOVED_EVENT_RESPONSE;
    }

    static bool isCounterEvent(short opCode) {
        return opCode ==  COUNTER_EVENT_RESPONSE;
    }
   };

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_HOTRODCONSTANTS_H
