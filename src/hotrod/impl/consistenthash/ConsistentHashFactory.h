#ifndef ISPN_HOTROD_CONSISTENT_HASH_FACTORY_H
#define  ISPN_HOTROD_CONSISTENT_HASH_FACTORY_H

#include "infinispan/hotrod/defs.h"
#include <map>

namespace infinispan {
namespace hotrod {
class Configuration;
namespace consistenthash {
class ConsistentHash;

class ConsistentHashFactory
{
  public:
    /*
     * We can not really dynamically load classes from configuration like in Java client
     * This method is therefore useless. Keep it as a note and remove later
     *
    void init(Configuration& config); */

    HR_SHARED_PTR<ConsistentHash> newConsistentHash(uint8_t version);
};
}}} // namespace infinispan::hotrod::consistenthash

#endif //  ISPN_HOTROD_CONSISTENT_HASH_FACTORY_H
