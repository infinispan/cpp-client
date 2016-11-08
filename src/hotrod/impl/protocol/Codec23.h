#ifndef ISPN_HOTROD_PROTOCOL_CODEC23_H
#define ISPN_HOTROD_PROTOCOL_CODEC23_H


#include <set>
#include <map>
#include <string>
#include "hotrod/impl/protocol/Codec22.h"

namespace infinispan {
namespace hotrod {
namespace protocol {

class Codec23: public Codec22 {

protected:
    Codec23()  { protocolVersion=HotRodConstants::VERSION_23; }

    friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC23_H
