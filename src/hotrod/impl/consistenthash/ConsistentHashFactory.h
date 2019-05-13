#ifndef ISPN_HOTROD_CONSISTENT_HASH_FACTORY_H
#define  ISPN_HOTROD_CONSISTENT_HASH_FACTORY_H

#include "infinispan/hotrod/defs.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
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
	static std::shared_ptr<ConsistentHash> newConsistentHash(uint8_t version);
};

}}} // namespace infinispan::hotrod::consistenthash

#endif //  ISPN_HOTROD_CONSISTENT_HASH_FACTORY_H
