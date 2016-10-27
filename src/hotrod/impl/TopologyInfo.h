#ifndef SRC_HOTROD_IMPL_TOPOLOGYINFO_H_
#define SRC_HOTROD_IMPL_TOPOLOGYINFO_H_

#include <infinispan/hotrod/CacheTopologyInfo.h>
#include <infinispan/hotrod/InetSocketAddress.h>
#include "hotrod/impl/Topology.h"
#include "hotrod/impl/consistenthash/ConsistentHashFactory.h"
#include "hotrod/sys/Log.h"
#include <memory>
#include <vector>
namespace infinispan {
namespace hotrod {
struct cmpCacheNameVector {
    bool operator()(const std::vector<char>& a, const std::vector<char>& b) const {
    	if (b.size()==0){
    		return false;
    	}
    	if (a.size()==0){
    		return true;
    	}
    	if (a.size()==b.size()){
          return strncmp(a.data(),b.data(),a.size()) < 0;
    	}
    	return (a.size()<b.size());
    }
};
class TopologyInfo {
public:
	TopologyInfo(int defaultTopologyId_, std::vector<transport::InetSocketAddress>& servers_, const Configuration& c_):
		defaultTopologyId(defaultTopologyId_), servers(servers_), configuration(c_) {
		const std::vector<char> emptyCacheName;
		topologyIdsByCache.insert(std::pair<std::vector<char>,int > (emptyCacheName, -1));
	    hashFactory.reset(new consistenthash::ConsistentHashFactory());

	};
	virtual ~TopologyInfo();
	Topology getTopology(const std::vector<char> &cacheName);
	CacheTopologyInfo getCacheTopologyInfo(const std::vector<char> &cacheName);
	std::shared_ptr<consistenthash::SegmentConsistentHash> updateTopology(std::vector<std::vector<transport::InetSocketAddress>>& segmentOwners,
	        uint32_t &numSegment, uint8_t &hashFunctionVersion, std::vector<char> cacheName, int topologyId);
	void updateTopology(
	        std::map<transport::InetSocketAddress, std::set<int32_t> >& servers2Hash,
	        int32_t numKeyOwners, uint8_t hashFunctionVersion, int32_t hashSpace, const std::vector<char>& cacheName);
	transport::InetSocketAddress getHashAwareServer(const std::vector<char>& key, const std::vector<char>& cacheName);
	void consistentHashErase(const std::vector<char>& cacheName)
	{
		consistentHashByCacheName.erase(cacheName);
	}
	std::vector<transport::InetSocketAddress>& getServers() const {
		return servers;
	}

	void updateServers(std::vector<transport::InetSocketAddress>& servers_){
		servers=servers_;
	}

	std::shared_ptr<infinispan::hotrod::consistenthash::ConsistentHashFactory> getHashFactory() const {
		return hashFactory;
	}
	int createTopologyId(std::vector<char> cacheName, int id)
	{
		 topologyIdsByCache.insert(std::pair<std::vector<char>, int> (cacheName, id));
		 return id;
	}
	int getSegmentsByCacheName(std::vector<char> cacheName);

	std::vector<transport::InetSocketAddress>& getServers()
    {
		return servers;
    }

private:
	Topology topology;
    std::shared_ptr<infinispan::hotrod::consistenthash::ConsistentHashFactory> hashFactory;
    std::map<std::vector<char>, std::shared_ptr<infinispan::hotrod::consistenthash::ConsistentHash>, cmpCacheNameVector > consistentHashByCacheName;
    std::map<std::vector<char>, uint32_t, cmpCacheNameVector > segmentsByCache;
    std::map<std::vector<char>, int , cmpCacheNameVector > topologyIdsByCache;
    int defaultTopologyId;
    std::vector<transport::InetSocketAddress>& servers;
    const Configuration& configuration;

    void traceEverythingOnTopology();

};

} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_TOPOLOGYINFO_H_ */
