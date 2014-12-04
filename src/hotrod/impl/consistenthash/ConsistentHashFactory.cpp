#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/consistenthash/ConsistentHashFactory.h"
#include "hotrod/impl/consistenthash/ConsistentHashV1.h"
#include "hotrod/impl/consistenthash/ConsistentHashV2.h"
#include "infinispan/hotrod/Configuration.h"

#include <iostream>

namespace infinispan {
namespace hotrod {
namespace consistenthash {
using infinispan::hotrod::protocol::HotRodConstants;

HR_SHARED_PTR<ConsistentHash> ConsistentHashFactory::newConsistentHash(uint8_t version) {
    HR_SHARED_PTR<ConsistentHash> result;
    switch (version) {
        case HotRodConstants::CONSISTENT_HASH_V1:
            result.reset(new ConsistentHashV1());
            break;
        case HotRodConstants::CONSISTENT_HASH_V2:
            result.reset(new ConsistentHashV2());
            break;
        default:
            break;
    }
    return result;
}

}}} // namespace
