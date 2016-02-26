#ifndef INCLUDE_INFINISPAN_HOTROD_CACHETOPOLOGYINFO_H_
#define INCLUDE_INFINISPAN_HOTROD_CACHETOPOLOGYINFO_H_

#include <vector>
#include <map>
#include "infinispan/hotrod/InetSocketAddress.h"
namespace infinispan {
namespace hotrod {

class CacheTopologyInfo {
	std::map<transport::InetSocketAddress,std::vector<int> > segmentPerServers;
	int numSegment;
	int topologyId;

public:
	CacheTopologyInfo(std::map<transport::InetSocketAddress,std::vector<int> > segmentPerServers_, int numSegment_, int topologyId_)
             : segmentPerServers(segmentPerServers_), numSegment(numSegment_), topologyId(topologyId_) {}
	int getNumSegment() {return numSegment; }
	int getTopologyId() { return topologyId; }
    std::map<transport::InetSocketAddress, std::vector<int> > getSegmentPerServer() { return segmentPerServers; }
	virtual ~CacheTopologyInfo() { }
};

} /* namespace hotrod */
} /* namespace infinispan */

#endif /* INCLUDE_INFINISPAN_HOTROD_CACHETOPOLOGYINFO_H_ */
