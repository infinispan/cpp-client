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
    TcpTransportFactory(const Configuration& config) : configuration(config), maxRetries(config.getMaxRetries()) {};
    void start(protocol::Codec& codec);
    void destroy();

    Transport& getTransport(const hrbytes& cacheName);
    Transport& getTransport(const hrbytes& key, const hrbytes& cacheName);

    void releaseTransport(Transport& transport);
    void invalidateTransport(
        const InetSocketAddress& address, Transport* transport);

    bool isTcpNoDelay();
    int getMaxRetries();
    int getSoTimeout();
    int getConnectTimeout();

    void updateServers(std::vector<InetSocketAddress>& );
    void updateHashFunction(
                std::map<InetSocketAddress, std::set<int32_t> >& servers2Hash,
                int32_t numKeyOwners, uint8_t hashFunctionVersion,
                int32_t hashSpace,
                const hrbytes& cacheName);
    void clearHashFunction(const hrbytes& cacheName);
    infinispan::hotrod::consistenthash::ConsistentHashFactory
            & getConsistentHashFactory();


  private:
    sys::Mutex lock;
    std::vector<InetSocketAddress> servers;
    const Configuration& configuration;
    int maxRetries;
    HR_SHARED_PTR<TransportObjectFactory> transportFactory;
    HR_SHARED_PTR<ConnectionPool> connectionPool;
    HR_SHARED_PTR<RequestBalancingStrategy> balancer;

    void createAndPreparePool();

    HR_SHARED_PTR<infinispan::hotrod::consistenthash::ConsistentHashFactory> hashFactory;

    std::map<hrbytes, HR_SHARED_PTR<infinispan::hotrod::consistenthash::ConsistentHash> > consistentHashByCacheName;
    void updateTransportCount();
    void pingServers();
    Transport& borrowTransportFromPool(const InetSocketAddress& server);
    ConnectionPool* getConnectionPool();
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_TCPTRANSPORTFACTORY_H */

