#ifndef ISPN_HOTROD_TRANSPORT_TCPTRANSPORTFACTORY_H
#define ISPN_HOTROD_TRANSPORT_TCPTRANSPORTFACTORY_H


#include "infinispan/hotrod/Configuration.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/transport/tcp/ConnectionPool.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"
#include "hotrod/impl/consistenthash/ConsistentHashFactory.h"
#include "hotrod/sys/Mutex.h"

#include <vector>

namespace infinispan {
namespace hotrod {
namespace transport {

class RequestBalancingStrategy;
class InetSocketAddress;

class TcpTransportFactory : public TransportFactory
{
  public:
    TcpTransportFactory(const Configuration& config) : configuration(config), maxRetries(config.getMaxRetries()), onFailover(false) {};
    void start(protocol::Codec& codec, int defaultTopologyId, ClientListenerNotifier* );
    void destroy();

    transport::Transport& getTransport(const std::vector<char>& cacheName, const std::set<transport::InetSocketAddress>& failedServers);
    transport::Transport& getTransport(const std::vector<char>& key, const std::vector<char>& cacheName, const std::set<transport::InetSocketAddress>& failedServers);

    void releaseTransport(Transport& transport);
    void invalidateTransport(
        const InetSocketAddress& address, Transport* transport);
    ClusterStatus clusterSwitch();
    ClusterStatus clusterSwitch(std::string clusterName);
    bool isTcpNoDelay();
    int getMaxRetries();
    int getSoTimeout();
    int getConnectTimeout();
    bool isSslEnabled();
    const std::string& getSslServerCAPath();
    const std::string& getSslServerCAFile();
    const std::string& getSslClientCertificateFile();

    void updateServers(std::vector<InetSocketAddress>& );
    void updateHashFunction(
                std::map<InetSocketAddress, std::set<int32_t> >& servers2Hash,
                int32_t numKeyOwners, uint8_t hashFunctionVersion,
                int32_t hashSpace,
                const std::vector<char>& cacheName);
    void updateHashFunction(
            std::vector<std::vector<InetSocketAddress>>& segmentOwners,
            uint32_t &numSegment, uint8_t &hashFunctionVersion,
            const std::vector<char>& cacheName, int topologyId);
    void clearHashFunction(const std::vector<char>& cacheName);
    infinispan::hotrod::consistenthash::ConsistentHashFactory
            & getConsistentHashFactory();


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
    Transport& borrowTransportFromPool(const InetSocketAddress& server);
    ConnectionPool* getConnectionPool();
    bool onFailover;
    std::vector<ServerConfiguration> getNextWorkingServersConfiguration();
    void pingExternalServer(InetSocketAddress s);
    ClientListenerNotifier* listenerNotifier;
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_TCPTRANSPORTFACTORY_H */

