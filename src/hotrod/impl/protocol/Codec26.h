#ifndef ISPN_HOTROD_PROTOCOL_CODEC26_H
#define ISPN_HOTROD_PROTOCOL_CODEC26_H


#include <set>
#include <map>
#include <string>
#include "hotrod/impl/protocol/Codec25.h"

namespace infinispan {
namespace hotrod {
namespace protocol {

class Codec26: public Codec25 {
public:
    virtual void writeClientListenerInterests(transport::Transport& t, unsigned char interestFlag) const;
protected:
    Codec26()  { protocolVersion=HotRodConstants::VERSION_26; }

    friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC26_H
