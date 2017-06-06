#ifndef ISPN_HOTROD_PROTOCOL_CODEC25_H
#define ISPN_HOTROD_PROTOCOL_CODEC25_H


#include <set>
#include <map>
#include <string>
#include "hotrod/impl/protocol/Codec24.h"

namespace infinispan {
namespace hotrod {
namespace protocol {

class Codec25: public Codec24 {

protected:
    Codec25()  { protocolVersion=HotRodConstants::VERSION_25; }

    friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC25_H
