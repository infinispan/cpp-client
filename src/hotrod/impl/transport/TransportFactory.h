#ifndef ISPN_HOTROD_TRANSPORT_TRANSPORTFACTORY_H
#define ISPN_HOTROD_TRANSPORT_TRANSPORTFACTORY_H


#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/Topology.h"
#include "hotrod/impl/TopologyInfo.h"
#include "infinispan/hotrod/Flag.h"
#include <hotrod/impl/event/ClientListenerNotifier.h>
#include <vector>
#include <set>
#include <map>

namespace infinispan {
namespace hotrod {
class RemoteCacheManagerImpl;
class Configuration;

namespace protocol {
class Codec;
}
namespace consistenthash{
class ConsistentHashFactory;
}

namespace transport {

class Transport;
class InetSocketAddress;

class TransportFactory
{
  friend class infinispan::hotrod::RemoteCacheManagerImpl;

  public:
    virtual void start(protocol::Codec& codec, int defaultTopologyId, ClientListenerNotifier*) = 0;
    virtual void destroy() = 0;

    virtual Transport& getTransport(const std::vector<char>& cacheName, const std::set<InetSocketAddress>& failedServers) = 0;
    virtual Transport& getTransport(const std::vector<char>& key, const std::vector<char>& cacheName, const std::set<InetSocketAddress>& failedServers) = 0;
    virtual bool clusterSwitch() = 0;
    virtual bool clusterSwitch(std::string) = 0;
    virtual void releaseTransport(Transport& transport) = 0;
    virtual void invalidateTransport(
        const InetSocketAddress& serverAddress, Transport* transport) = 0;

    virtual bool isTcpNoDelay() = 0;
    virtual int getMaxRetries() = 0;
    virtual int getSoTimeout() = 0;
    virtual int getConnectTimeout() = 0;
    virtual const std::string& getSniHostName() = 0;

    virtual void updateServers(std::vector<InetSocketAddress>& ) = 0;
    virtual ~TransportFactory() {}

    virtual void updateHashFunction( std::vector<std::vector<InetSocketAddress>>& segmentOwners,
            						uint32_t &numSegment, uint8_t &hashFunctionVersion,
            						const std::vector<char>& cacheName, int topologyId) = 0;

    virtual void clearHashFunction(const std::vector<char>& cacheName) = 0;

    virtual infinispan::hotrod::consistenthash::ConsistentHashFactory
    & getConsistentHashFactory() = 0;

    int getTopologyId(const std::vector<char> &cacheName) {
    	if (topologyInfo) {
    	  return topologyInfo->getCacheTopologyInfo(cacheName).getTopologyId();
    	}
    	else {
    		throw Exception("TopologyInfo is null");
    	}
    }
    int createTopologyId(std::vector<char> cacheName) {
    	return topologyInfo->createTopologyId(cacheName,-1);
    }
    int getTopologyAge()
    {
    	return topologyAge;
    }

    CacheTopologyInfo getCacheTopologyInfo(const std::vector<char>& cacheName)
    {
    	return topologyInfo->getCacheTopologyInfo(cacheName);
    }

    TopologyInfo* getTopologyInfo() { return topologyInfo; }

    // TODO: ssl
    // getSSLContext
  protected:
    TopologyInfo* topologyInfo;
    int topologyAge;
    const std::string sniHostName;

  private:
    static TransportFactory* newInstance(const Configuration& config);

};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_TRANSPORTFACTORY_H */

