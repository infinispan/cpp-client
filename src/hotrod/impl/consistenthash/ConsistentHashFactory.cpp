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

ConsistentHashFactory::ConsistentHashFactory() {
    int8_t key  = HotRodConstants::CONSISTENT_HASH_V1;
    consistentHashMap[key] = new ConsistentHashV1();
    key = HotRodConstants::CONSISTENT_HASH_V2;
    consistentHashMap[key] = new ConsistentHashV2();
}

ConsistentHashFactory::~ConsistentHashFactory() {
    for (std::map<uint8_t, ConsistentHash*>::iterator it =
            consistentHashMap.begin(); it != consistentHashMap.end(); ++it) {
        delete it->second;
    }
}

ConsistentHash* ConsistentHashFactory::newConsistentHash(uint8_t version) {
    ConsistentHash* result = ConsistentHashFactory::consistentHashMap[version];
    return result;
}

}}} // namespace
