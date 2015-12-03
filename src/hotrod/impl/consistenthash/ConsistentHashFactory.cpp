#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/consistenthash/ConsistentHashFactory.h"
#include "hotrod/impl/consistenthash/ConsistentHashV2.h"
#include "infinispan/hotrod/Configuration.h"
#include "infinispan/hotrod/exceptions.h"

#include <iostream>

namespace infinispan {
namespace hotrod {
namespace consistenthash {
using infinispan::hotrod::protocol::HotRodConstants;

std::shared_ptr<ConsistentHash> ConsistentHashFactory::newConsistentHash(uint8_t version) {
    std::shared_ptr<ConsistentHash> result;
    switch (version) {
        case HotRodConstants::CONSISTENT_HASH_V2:
            result.reset(new ConsistentHashV2());
            break;
        default:
		throw UnsupportedOperationException();
            break;
    }
    return result;
}

}}} // namespace
