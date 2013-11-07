#ifndef ISPN_HOTROD_CONSISTENT_HASH_H
#define ISPN_HOTROD_CONSISTENT_HASH_H

#include "infinispan/hotrod/types.h"
#include "infinispan/hotrod/Hash.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"
#include <set>
#include <map>

namespace infinispan {
namespace hotrod {
namespace consistenthash {

class ConsistentHash {

public:
	virtual ~ConsistentHash(){}
    virtual void init(
            std::map<infinispan::hotrod::transport::InetSocketAddress,
                    std::set<int32_t> > & servers2Hash, int32_t numKeyOwners,
            int32_t hashSpace) = 0;

    virtual const infinispan::hotrod::transport::InetSocketAddress& getServer(const hrbytes& key) = 0;

    /**
     * Computes hash code of a given int32_t and then normalizes it to ensure a positive
     * value is always returned.
     * @param uint32_t to hash
     * @return a non-null, non-negative normalized hash code for a given object
     */
    virtual int32_t getNormalizedHash(int32_t objectId) = 0;

    virtual int32_t getNormalizedHash(const hrbytes& key) = 0;

protected:
    Hash *hash;
};

}}} // namespace infinispan::hotrod::consistenthash

#endif // ISPN_HOTROD_CONSISTENT_HASH_H
