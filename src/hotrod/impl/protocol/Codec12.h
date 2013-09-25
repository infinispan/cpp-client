#ifndef ISPN_HOTROD_PROTOCOL_CODEC12_H
#define ISPN_HOTROD_PROTOCOL_CODEC12_H

#include "hotrod/impl/protocol/Codec11.h"

#include <set>
#include <map>
#include <string>

namespace infinispan {
namespace hotrod {
namespace protocol {

class Codec12: public Codec11 {

public:
    HeaderParams& writeHeader(
            infinispan::hotrod::transport::Transport& transport,
            HeaderParams& params) const;

    Codec12() {
    }

    friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC12_H
