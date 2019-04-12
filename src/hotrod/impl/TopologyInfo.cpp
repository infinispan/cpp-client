#include <hotrod/impl/TopologyInfo.h>
#include "hotrod/impl/consistenthash/ConsistentHashV2.h"
#include "hotrod/impl/consistenthash/SegmentConsistentHash.h"

#include <map>
#include <numeric>

namespace infinispan {
namespace hotrod {


TopologyInfo::~TopologyInfo() {
	// TODO Auto-generated destructor stub
}

std::shared_ptr<consistenthash::SegmentConsistentHash> TopologyInfo::updateTopology(std::vector<std::vector<transport::InetSocketAddress>>& segmentOwners,
        uint32_t &numSegment, uint8_t &hashFunctionVersion, std::vector<char> cacheName, int topologyId) {
	std::shared_ptr<consistenthash::SegmentConsistentHash> hash= hashFactory->newSegmentConsistentHash(hashFunctionVersion);
	if (hash)
	{
		hash->seg_init(segmentOwners, numSegment);
	}
	createTopologyId(cacheName, topologyId);
	segmentsByCache[cacheName]=numSegment;
	topologyIdsByCache[cacheName]=topologyId;
	consistentHashByCacheName[cacheName]=hash;
	return hash;
	}

Topology TopologyInfo::getTopology(const std::vector<char> &/*cacheName*/) {
  return Topology();
}

void TopologyInfo::traceEverythingOnTopology()
{
	   TRACE("traceEveryThingOnTopology %p: consistentHashByCacheName",this);
	   for (auto it=consistentHashByCacheName.begin(); it!=consistentHashByCacheName.end(); ++it)
	   {
		   TRACE("key: %.16s", it->first);
		   TRACE("value: %p", (void *)it->second.get());
	   }

	   TRACE("traceEveryThingOnTopology: segmentsByCache");
	   for (auto it=segmentsByCache.begin(); it!=segmentsByCache.end(); ++it)
	   {
		   TRACE("key: %.16s", it->first);
		   TRACE("value: %d", it->second);
	   }

	   TRACE("traceEveryThingOnTopology: topologyIdsByCache");
	   for (auto it=topologyIdsByCache.begin(); it!=topologyIdsByCache.end(); ++it)
	   {
		   TRACE("key: %.16s", it->first);
		   TRACE("value: %d",  it->second);
	   }
}

CacheTopologyInfo TopologyInfo::getCacheTopologyInfo(const std::vector<char>& cacheName) {

	  //traceEverythingOnTopology();
      if (consistentHashByCacheName.find(cacheName)!=consistentHashByCacheName.end())
      {
        return CacheTopologyInfo(consistentHashByCacheName[cacheName]->getSegmentsByServers(), segmentsByCache[cacheName], topologyIdsByCache[cacheName]);
      }
      else {
    	std::map<transport::InetSocketAddress,std::vector<int> > segmentPerServers_;
    	for( auto it= servers.begin(); it<servers.end(); it ++){
    		std::vector<int> v(segmentsByCache[cacheName]);
    		std::iota(v.begin(),v.end(),0);
    		segmentPerServers_[*it]=v;
    	}
          return CacheTopologyInfo(segmentPerServers_, segmentsByCache[cacheName], topologyIdsByCache[cacheName]);
      }
}


transport::InetSocketAddress TopologyInfo::getHashAwareServer(
		const std::vector<char>& key, const std::vector<char>& cacheName) {
	std::shared_ptr<infinispan::hotrod::consistenthash::ConsistentHash> consistentHash;

    std::map<std::vector<char>, std::shared_ptr<infinispan::hotrod::consistenthash::ConsistentHash> >::iterator element = consistentHashByCacheName.find(cacheName);
    if (element != consistentHashByCacheName.end()) {
        consistentHash = element->second;
    }
    if (!consistentHash) {
    	return transport::InetSocketAddress();
    }
    else {
        return consistentHash->getServer(key);
    }


}

} /* namespace hotrod */
} /* namespace infinispan */
