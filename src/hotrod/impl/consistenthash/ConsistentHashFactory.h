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
     * Constructor/Destructor are public as we need to
     * test ConsistentHashFactory
     */
    ConsistentHashFactory();
    ~ConsistentHashFactory();

    /*
     * We can not really dynamically load classes from configuration like in Java client
     * This method is therefore useless. Keep it as a note and remove later
     *
    void init(Configuration& config); */

    ConsistentHash* newConsistentHash(uint8_t version);


  private:
    std::map<uint8_t, ConsistentHash*> consistentHashMap;
};
}}} // namespace infinispan::hotrod::consistenthash

#endif //  ISPN_HOTROD_CONSISTENT_HASH_FACTORY_H
