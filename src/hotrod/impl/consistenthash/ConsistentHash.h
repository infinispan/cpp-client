#ifndef ISPN_HOTROD_CONSISTENT_HASH_H
#define ISPN_HOTROD_CONSISTENT_HASH_H

#include <infinispan/hotrod/InetSocketAddress.h>
#include "infinispan/hotrod/defs.h"
#include "hotrod/impl/hash/Hash.h"
#include <set>
#include <map>
#include <vector>

namespace infinispan {
namespace hotrod {
namespace consistenthash {

class ConsistentHash {

public:
    virtual ~ConsistentHash() { if (hash) delete hash; }
    virtual void init(
            std::map<infinispan::hotrod::transport::InetSocketAddress,
                    std::set<int32_t> > & servers2Hash, int32_t numKeyOwners,
            int32_t hashSpace) = 0;

    virtual const infinispan::hotrod::transport::InetSocketAddress& getServer(const std::vector<char>& key) = 0;

    /**
     * Computes hash code of a given int32_t and then normalizes it to ensure a positive
     * value is always returned.
     * @param uint32_t to hash
     * @return a non-null, non-negative normalized hash code for a given object
     */
    virtual int32_t getNormalizedHash(int32_t objectId) = 0;

    virtual int32_t getNormalizedHash(const std::vector<char>& key) = 0;

    virtual std::map<infinispan::hotrod::transport::InetSocketAddress, std::vector<int> > getSegmentsByServers(){
		// No segments here. Return empty map
    	std::map<infinispan::hotrod::transport::InetSocketAddress, std::vector<int> > m;
    	return m;
    }

protected:
    ConsistentHash(Hash* hash_) : hash(hash_) {}

    Hash* hash;
};

}}} // namespace infinispan::hotrod::consistenthash

#endif // ISPN_HOTROD_CONSISTENT_HASH_H
