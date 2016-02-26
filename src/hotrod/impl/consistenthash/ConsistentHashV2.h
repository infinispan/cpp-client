#ifndef ISPN_HOTROD_CONSISTENTHASHV2_H_
#define ISPN_HOTROD_CONSISTENTHASHV2_H_

#include "hotrod/impl/consistenthash/ConsistentHash.h"
#include <vector>

namespace infinispan {
namespace hotrod {
namespace transport {
class InetSocketAddress;
}
namespace consistenthash {

class ConsistentHashV2: public ConsistentHash {
public:
    void init(
            std::map<infinispan::hotrod::transport::InetSocketAddress,
                    std::set<int32_t> > & servers2Hash, int32_t numKeyOwners,
            int32_t hashSpace);
    void init(std::vector<std::vector<transport::InetSocketAddress>>& segmentOwners, uint32_t numSegment);

    const infinispan::hotrod::transport::InetSocketAddress& getServer(const std::vector<char>& key);

    int32_t getNormalizedHash(int32_t objectId);
    int32_t getNormalizedHash(const std::vector<char>& key);

private:
    int32_t getHashIndex(int32_t normalisedHashForKey);

    std::map<int32_t, infinispan::hotrod::transport::InetSocketAddress> positions;
    std::vector<infinispan::hotrod::transport::InetSocketAddress> addresses;
    std::vector<int32_t> hashes;
    int32_t hashSpace;
    int32_t numKeyOwners;
    bool hashSpaceIsMaxInt;

protected:
    ConsistentHashV2();
    friend class ConsistentHashFactory;

};
}}} // namespace infinispan::hotrod::consistenthash
#endif /* ISPN_HOTROD_CONSISTENTHASHV2_H_ */
