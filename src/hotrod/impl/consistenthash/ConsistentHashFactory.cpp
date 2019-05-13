#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/consistenthash/ConsistentHashFactory.h"
#include "hotrod/impl/consistenthash/SegmentConsistentHash.h"
#include "infinispan/hotrod/Configuration.h"
#include "infinispan/hotrod/exceptions.h"

#include <iostream>

namespace infinispan {
namespace hotrod {
namespace consistenthash {

std::shared_ptr<ConsistentHash> ConsistentHashFactory::newConsistentHash(uint8_t version) {
    std::shared_ptr<ConsistentHash> result;
    switch (version) {
        case HotRodConstants::SEGMENT_CONSISTENT_HASH:
            result.reset(new SegmentConsistentHash());
            break;
        default:
		throw UnsupportedOperationException();
            break;
    }
    return result;
}


}}} // namespace
