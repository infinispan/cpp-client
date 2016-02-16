#include "hotrod/impl/consistenthash/ConsistentHashV2.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"
#include "hotrod/impl/hash/MurmurHash3.h"
#include <limits>
#include <algorithm>

namespace infinispan {
namespace hotrod {
using transport::InetSocketAddress;
namespace consistenthash {

ConsistentHashV2::ConsistentHashV2(): ConsistentHash(new MurmurHash3()), hashSpace(0), numKeyOwners(0), hashSpaceIsMaxInt(false) {}

void ConsistentHashV2::init(
        std::map<InetSocketAddress, std::set<int32_t> > & servers2Hash, int32_t nKeyOwners,
        int32_t hSpace) {

    positions.clear();
    for (std::map<InetSocketAddress, std::set<int32_t> >::iterator outer_it =
            servers2Hash.begin(); outer_it != servers2Hash.end(); ++outer_it) {

        for (std::set<int32_t>::iterator inner_it = outer_it->second.begin(); inner_it
                != outer_it->second.end(); ++inner_it) {
            positions.insert(
                    std::pair<int32_t, InetSocketAddress>(*inner_it,
                            outer_it->first));
        }
    }

    int32_t hashWheelSize = (int32_t) positions.size();

    hashes.clear();
    hashes.reserve(hashWheelSize);

    addresses.clear();
    addresses.reserve(hashWheelSize);

    for (std::map<int32_t, InetSocketAddress>::iterator it = positions.begin(); it
            != positions.end(); ++it) {
        hashes.push_back(it->first);
        addresses.push_back(it->second);
    }

    this->hashSpace = hSpace;

    //TODO
    // This is true if we're talking to an instance of Infinispan 5.2 or newer.
    this->hashSpaceIsMaxInt = hSpace == std::numeric_limits<int32_t>::max();

    this->numKeyOwners = nKeyOwners;
}

const InetSocketAddress& ConsistentHashV2::getServer(const hrbytes& key) {
    int32_t normalisedHashForKey;
    if (hashSpaceIsMaxInt) {
        normalisedHashForKey = getNormalizedHash(key);
        if (normalisedHashForKey == std::numeric_limits<int32_t>::max()) {
            normalisedHashForKey = 0;
        }
    } else {
        normalisedHashForKey = getNormalizedHash(key) % hashSpace;
    }

    int32_t mainOwner = getHashIndex(normalisedHashForKey);

    int32_t indexToReturn = mainOwner % hashes.size();
    return addresses.at(indexToReturn);
}

int32_t ConsistentHashV2::getNormalizedHash(int32_t objectId) {
    // make sure no negative numbers are involved.
    return hash->hash(objectId) & std::numeric_limits<int32_t>::max();
}

int32_t ConsistentHashV2::getNormalizedHash(const hrbytes& key) {
    // make sure no negative numbers are involved.
    return hash->hash(key.data(), key.size()) & std::numeric_limits<int32_t>::max();
}

int32_t ConsistentHashV2::getHashIndex(int32_t normalisedHashForKey) {
    std::vector<int32_t>::iterator low = std::lower_bound(hashes.begin(),
            hashes.end(), normalisedHashForKey);
    if (low != hashes.end()) {
        /*TODO revisit but it seems low - hashes.begin();
         * works for both cases:
         * 1) when key is found
         * 2) when key is not found but iterator is pointing to the first
         * element in the range [first,last) which does not compare
         * less than normalisedHashForKey.
         *
         * if (*low == normalisedHashForKey) {
         //found the element
         return low - hashes.begin();
         } else {
         return (low - hashes.begin()) - 1;
         }*/
        return (int32_t) (low - hashes.begin());
    } else {
        return 0;
    }
}

}}} // namespace infinispan::hotrod::consistenthash

