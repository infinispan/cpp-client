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

class TopologyInfo {
public:
	TopologyInfo(int defaultTopologyId_, std::vector<transport::InetSocketAddress>& servers_, const Configuration& c_):
		defaultTopologyId(defaultTopologyId_), servers(servers_), configuration(c_) {
		const std::vector<char> emptyCacheName;
		topologyIdsByCache.insert(std::pair<std::vector<char>,int > (emptyCacheName, -1));
	};
	virtual ~TopologyInfo();
	CacheTopologyInfo getCacheTopologyInfo(const std::vector<char> &cacheName);
	std::shared_ptr<consistenthash::ConsistentHash> updateTopology(std::vector<std::vector<transport::InetSocketAddress>>& segmentOwners,
	        uint32_t &numSegment, uint8_t &hashFunctionVersion, std::vector<char> cacheName, int topologyId);
	void updateTopology(
	        std::map<transport::InetSocketAddress, std::set<int32_t> >& servers2Hash,
	        int32_t numKeyOwners, uint8_t hashFunctionVersion, int32_t hashSpace, const std::vector<char>& cacheName);
	transport::InetSocketAddress getHashAwareServer(const std::vector<char>& key, const std::vector<char>& cacheName);
	std::vector<transport::InetSocketAddress>& getServers() const {
		return servers;
	}

	void updateServers(std::vector<transport::InetSocketAddress>& servers_){
		servers=servers_;
	}

	int createTopologyId(std::vector<char> cacheName, int id)
	{
		 topologyIdsByCache.insert(std::pair<std::vector<char>, int> (cacheName, id));
		 return id;
	}
	int getSegmentsByCacheName(std::vector<char> cacheName);

private:
	Topology topology;
    std::map<std::vector<char>, std::shared_ptr<infinispan::hotrod::consistenthash::ConsistentHash> > consistentHashByCacheName;
    std::map<std::vector<char>, uint32_t > segmentsByCache;
    std::map<std::vector<char>, int > topologyIdsByCache;
    int defaultTopologyId;
    std::vector<transport::InetSocketAddress>& servers;
    const Configuration& configuration;

    void traceEverythingOnTopology();
    static std::shared_ptr<consistenthash::ConsistentHash> newConsistentHash(uint8_t hashFunctionVersion) {
    	return infinispan::hotrod::consistenthash::ConsistentHashFactory::newConsistentHash(hashFunctionVersion);
    }
};

} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_TOPOLOGYINFO_H_ */
