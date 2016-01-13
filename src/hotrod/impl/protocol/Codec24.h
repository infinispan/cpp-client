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

public:
    HeaderParams& writeHeader(
            infinispan::hotrod::transport::Transport& transport,
            HeaderParams& params) const;

    Codec24() {
    }

    friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC24_H
