#ifndef ISPN_HOTROD_CONSISTENTHASHV1_H_
#define ISPN_HOTROD_CONSISTENTHASHV1_H_

#include "hotrod/impl/consistenthash/ConsistentHash.h"
#include <vector>

namespace infinispan {
namespace hotrod {
namespace transport {
class InetSocketAddress;
}
namespace consistenthash {

class ConsistentHashV1: public ConsistentHash {
public:
    void init(
            std::map<infinispan::hotrod::transport::InetSocketAddress,
                    std::set<int32_t> > & servers2Hash, int32_t numKeyOwners,
            int32_t hashSpace);

    const infinispan::hotrod::transport::InetSocketAddress& getServer(const hrbytes& key);

    int32_t getNormalizedHash(int32_t objectId);
    int32_t getNormalizedHash(const hrbytes& key);

private:
    int32_t getHashIndex(int32_t normalisedHashForKey);
    friend class ConsistentHashFactory;

    std::map<int32_t, infinispan::hotrod::transport::InetSocketAddress> positions;
    std::vector<infinispan::hotrod::transport::InetSocketAddress> addresses;
    std::vector<int32_t> hashes;
    int32_t hashSpace;
    int32_t numKeyOwners;
    bool hashSpaceIsMaxInt;

protected:
    ConsistentHashV1();

};
}}} // namespace infinispan::hotrod::consistenthash

#endif /* ISPN_HOTROD_CONSISTENTHASHV1_H_ */
