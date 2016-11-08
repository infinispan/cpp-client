#ifndef ISPN_HOTROD_PROTOCOL_CODEC24_H
#define ISPN_HOTROD_PROTOCOL_CODEC24_H


#include <set>
#include <map>
#include <string>
#include "hotrod/impl/protocol/Codec23.h"

namespace infinispan {
namespace hotrod {
namespace protocol {

class Codec24: public Codec23 {

protected:
    Codec24()  { protocolVersion=HotRodConstants::VERSION_24; }

    friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC24_H
