#include <hotrod/impl/TopologyInfo.h>
#include "hotrod/impl/consistenthash/SegmentConsistentHash.h"


#include <map>
#include <numeric>

namespace infinispan {
namespace hotrod {


TopologyInfo::~TopologyInfo() {
	// TODO Auto-generated destructor stub
}

void TopologyInfo::updateTopology(std::vector<std::vector<transport::InetSocketAddress>>& segmentOwners,
        uint32_t &numSegment, uint8_t &hashFunctionVersion, std::vector<char> cacheName, int topologyId) {
	std::unique_lock<std::mutex> ulh(mutexHash, std::defer_lock);
	std::unique_lock<std::mutex> uls(mutexSeg, std::defer_lock);
	std::lock(ulh, uls);
	std::shared_ptr<consistenthash::ConsistentHash> hash(new infinispan::hotrod::consistenthash::SegmentConsistentHash());
	hash->init(segmentOwners, numSegment);
	createTopologyId(cacheName, topologyId);
	segmentsByCache[cacheName]=numSegment;
	topologyIdsByCache[cacheName]=topologyId;
	consistentHashByCacheName[cacheName]=hash;
	}

CacheTopologyInfo TopologyInfo::getCacheTopologyInfo(const std::vector<char>& cacheName) {
	std::unique_lock<std::mutex> ul(mutexHash);
	if (consistentHashByCacheName.find(cacheName)
			!= consistentHashByCacheName.end()) {
		return CacheTopologyInfo(
				consistentHashByCacheName[cacheName]->getSegmentsByServers(),
				segmentsByCache[cacheName], topologyIdsByCache[cacheName]);
	} else {
		std::map<transport::InetSocketAddress, std::vector<int> > segmentPerServers_;
		for (auto it = servers.begin(); it < servers.end(); it++) {
			std::vector<int> v(segmentsByCache[cacheName]);
			std::iota(v.begin(), v.end(), 0);
			segmentPerServers_[*it] = v;
		}
		return CacheTopologyInfo(segmentPerServers_, segmentsByCache[cacheName],
				topologyIdsByCache[cacheName]);
	}
}


transport::InetSocketAddress TopologyInfo::getHashAwareServer(
		const std::vector<char> &key, const std::vector<char> &cacheName) {
	std::unique_lock<std::mutex> ulh(mutexHash);
	std::map<std::vector<char>,
			std::shared_ptr<infinispan::hotrod::consistenthash::ConsistentHash> >::iterator element =
			consistentHashByCacheName.find(cacheName);
	if (element != consistentHashByCacheName.end()) {
		if (!element->second) {
			return element->second->getServer(key);
		}
	}
	return transport::InetSocketAddress();
}

} /* namespace hotrod */
} /* namespace infinispan */
