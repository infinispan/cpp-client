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

std::shared_ptr<consistenthash::ConsistentHash> TopologyInfo::updateTopology(std::vector<std::vector<transport::InetSocketAddress>>& segmentOwners,
        uint32_t &numSegment, uint8_t &hashFunctionVersion, std::vector<char> cacheName, int topologyId) {
	std::shared_ptr<consistenthash::ConsistentHash> hash= hashFactory->newConsistentHash(hashFunctionVersion);
	if (hash)
	{
		hash->init(segmentOwners, numSegment);
	}
	createTopologyId(cacheName, topologyId);
	segmentsByCache[cacheName]=numSegment;
	topologyIdsByCache.insert(std::pair<std::vector<char>, int> (cacheName, topologyId));
	consistentHashByCacheName[cacheName]=hash;
	return hash;
	}

void TopologyInfo::updateTopology(
		std::map<transport::InetSocketAddress, std::set<int32_t> >& servers2Hash,
		int32_t numKeyOwners, uint8_t hashFunctionVersion, int32_t hashSpace,
		const std::vector<char>& cacheName) {
    std::shared_ptr<consistenthash::ConsistentHash> hash = hashFactory->newConsistentHash(hashFunctionVersion);
    if (hash == NULL) {
        std::cout << "updateHashFunction with hash version "
            << hashFunctionVersion << " failed!" << std::endl;
    } else {
        hash->init(servers2Hash, numKeyOwners, hashSpace);
    }
      consistentHashByCacheName[cacheName] = hash;
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
    		segmentPerServers_.insert( std::pair<transport::InetSocketAddress, std::vector<int> >(*it,v));
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

int TopologyInfo::getSegmentsByCacheName(std::vector<char> cacheName) {
    std::map<std::vector<char>,unsigned int>::iterator it=segmentsByCache.find(cacheName);
    TRACE("getSegmentsByCacheName(%.16s)", cacheName.data());
    if (it!=segmentsByCache.end()){
		return it->second;
    }
		else{
			return 0;
			}
}

} /* namespace hotrod */
} /* namespace infinispan */
