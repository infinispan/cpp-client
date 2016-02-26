#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/consistenthash/ConsistentHashFactory.h"
#include "hotrod/impl/consistenthash/ConsistentHashV2.h"
#include "hotrod/impl/consistenthash/SegmentConsistentHash.h"
#include "infinispan/hotrod/Configuration.h"
#include "infinispan/hotrod/exceptions.h"

#include <iostream>

namespace infinispan {
namespace hotrod {
namespace consistenthash {
std::shared_ptr<ConsistentHash> ConsistentHashFactory::newConsistentHash(uint8_t version) const {
    std::shared_ptr<ConsistentHash> result;
    switch (version) {
        case HotRodConstants::CONSISTENT_HASH_V2:
            result.reset(new ConsistentHashV2());
            break;
        case HotRodConstants::SEGMENT_CONSISTENT_HASH:
            result.reset(new SegmentConsistentHash());
            break;
        default:
		throw UnsupportedOperationException();
            break;
    }
    return result;
}

std::shared_ptr<SegmentConsistentHash> ConsistentHashFactory::newSegmentConsistentHash(uint8_t version) const {
    std::shared_ptr<SegmentConsistentHash> result;
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
