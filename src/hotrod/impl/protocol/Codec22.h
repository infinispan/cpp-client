#ifndef ISPN_HOTROD_PROTOCOL_CODEC22_H
#define ISPN_HOTROD_PROTOCOL_CODEC22_H


#include <set>
#include <map>
#include <string>
#include "hotrod/impl/protocol/Codec21.h"

namespace infinispan {
namespace hotrod {
namespace protocol {

class Codec22: public Codec21 {

public:
    HeaderParams& writeHeader(
            infinispan::hotrod::transport::Transport& transport,
            HeaderParams& params) const;
    void writeExpirationParams(transport::Transport& t,uint64_t lifespan, uint64_t maxIdle) const;

    Codec22() {
    }

    friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC22_H
