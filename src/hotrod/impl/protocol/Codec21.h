#ifndef ISPN_HOTROD_PROTOCOL_CODEC21_H
#define ISPN_HOTROD_PROTOCOL_CODEC21_H


#include <set>
#include <map>
#include <string>
#include "hotrod/impl/protocol/Codec20.h"

namespace infinispan {
namespace hotrod {
namespace protocol {

class Codec21: public Codec20 {

public:
    virtual void writeClientListenerParams(transport::Transport& t, const ClientListener& clientListener,
    		const std::vector<std::vector<char> > &filterFactoryParams, const std::vector<std::vector<char> > &converterFactoryParams) const;

    virtual ClientCacheEntryExpiredEvent<std::vector<char>> readExpiredEvent(transport::Transport &transport) const;

protected:
    Codec21() { protocolVersion=HotRodConstants::VERSION_21; }

friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC21_H
