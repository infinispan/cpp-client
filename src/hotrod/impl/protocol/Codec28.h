#ifndef ISPN_HOTROD_PROTOCOL_CODEC28_H
#define ISPN_HOTROD_PROTOCOL_CODEC28_H


#include <set>
#include <map>
#include <string>
#include "hotrod/impl/protocol/Codec27.h"

namespace infinispan {
namespace hotrod {
namespace protocol {

class Codec28: public Codec27 {
protected:
    Codec28()  { protocolVersion=HotRodConstants::VERSION_28; }
    HeaderParams& writeHeader(infinispan::hotrod::transport::Transport& transport, HeaderParams& params) const;
    friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC28_H
