#ifndef ISPN_HOTROD_TRANSPORT_TCPTRANSPORTFACTORY_H
#define ISPN_HOTROD_TRANSPORT_TCPTRANSPORTFACTORY_H


#include "infinispan/hotrod/Configuration.h"
#include "infinispan/hotrod/Flag.h"
#include "hotrod/impl/Topology.h"
#include "hotrod/impl/TopologyInfo.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/tcp/ConnectionPool.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"
#include "hotrod/impl/consistenthash/ConsistentHashFactory.h"
#include "hotrod/sys/Mutex.h"
#include "hotrod/impl/event/ClientListenerNotifier.h"
#include "infinispan/hotrod/exceptions.h"
#include <vector>
#include <set>
#include <map>

namespace infinispan {
namespace hotrod {

class RequestBalancingStrategy;
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
    TransportFactory(const Configuration& config) : topologyInfo(initialServers, config), configuration(config), maxRetries(config.getMaxRetries()) {}
    void start(protocol::Codec& codec, ClientListenerNotifier* );
    void destroy();

    transport::Transport& getTransport(const std::vector<char>& cacheName, const std::set<transport::InetSocketAddress>& failedServers);
    transport::Transport& getTransport(const std::vector<char>& key, const std::vector<char>& cacheName, const std::set<transport::InetSocketAddress>& failedServers);

    void releaseTransport(Transport& transport);
    void invalidateTransport(
        const InetSocketAddress& address, Transport* transport);
    bool clusterSwitch();
    bool clusterSwitch(std::string clusterName);
    bool isTcpNoDelay();
    int getMaxRetries();
    int getSoTimeout();
    int getConnectTimeout();
    const std::string& getSniHostName();
    bool isSslEnabled();
    const std::string& getSslServerCAPath();
    const std::string& getSslServerCAFile();
    const std::string& getSslClientCertificateFile();

    void updateServers(std::vector<InetSocketAddress>& );
    void updateHashFunction(
            std::vector<std::vector<InetSocketAddress>>& segmentOwners,
            uint32_t &numSegment, uint8_t &hashFunctionVersion,
            const std::vector<char>& cacheName, int topologyId);
    Transport& borrowTransportFromPool(const InetSocketAddress& server);
    ~TransportFactory() { }
    const Configuration& getConfiguration() { return configuration; }
    int getTopologyId(const std::vector<char> &cacheName) {
    	  return topologyInfo.getCacheTopologyInfo(cacheName).getTopologyId();
    }
    int createTopologyId(std::vector<char> cacheName) {
    	return topologyInfo.createTopologyId(cacheName,-1);
    }
    int getTopologyAge()
    {
    	return topologyAge;
    }

    CacheTopologyInfo getCacheTopologyInfo(const std::vector<char>& cacheName)
    {
    	return topologyInfo.getCacheTopologyInfo(cacheName);
    }

    TopologyInfo& getTopologyInfo() { return topologyInfo; }

  protected:
    TopologyInfo topologyInfo;
    int topologyAge;
    std::string sniHostName;

  private:
    sys::Mutex lock;
    std::vector<InetSocketAddress> initialServers;
    const Configuration& configuration;
    int maxRetries;
    std::shared_ptr<TransportObjectFactory> transportFactory;
    std::shared_ptr<ConnectionPool> connectionPool;
    std::shared_ptr<FailOverRequestBalancingStrategy> balancer;
    std::string currCluster;
    void createAndPreparePool();
    void updateTransportCount();
    void pingServers();
    ConnectionPool* getConnectionPool();
    std::vector<ServerConfiguration> getNextWorkingServersConfiguration();
    void pingExternalServer(InetSocketAddress s);
    ClientListenerNotifier* listenerNotifier;
    static TransportFactory* newInstance(const Configuration& config);
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_TCPTRANSPORTFACTORY_H */

