#ifndef ISPN_HOTROD_PROTOCOL_CODEC27_H
#define ISPN_HOTROD_PROTOCOL_CODEC27_H


#include <set>
#include <map>
#include <string>
#include "hotrod/impl/protocol/Codec26.h"

namespace infinispan {
namespace hotrod {
namespace protocol {

class Codec27: public Codec26 {
protected:
    Codec27()  { protocolVersion=HotRodConstants::VERSION_27; }

    friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC27_H
