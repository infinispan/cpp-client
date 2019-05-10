#ifndef ISPN_HOTROD_CONSISTENT_HASH_FACTORY_H
#define  ISPN_HOTROD_CONSISTENT_HASH_FACTORY_H

#include "infinispan/hotrod/defs.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/consistenthash/ConsistentHashV2.h"
#include "hotrod/impl/consistenthash/SegmentConsistentHash.h"
#include "infinispan/hotrod/Configuration.h"
#include "infinispan/hotrod/exceptions.h"

#include <map>

namespace infinispan {
namespace hotrod {
class Configuration;
namespace consistenthash {
class ConsistentHash;
using infinispan::hotrod::protocol::HotRodConstants;

class ConsistentHashFactory
{
  public:
    /*
     * We can not really dynamically load classes from configuration like in Java client
     * This method is therefore useless. Keep it as a note and remove later
     *
    void init(Configuration& config); */

	std::shared_ptr<ConsistentHash> newConsistentHash(uint8_t version) const;
};

}}} // namespace infinispan::hotrod::consistenthash

#endif //  ISPN_HOTROD_CONSISTENT_HASH_FACTORY_H
