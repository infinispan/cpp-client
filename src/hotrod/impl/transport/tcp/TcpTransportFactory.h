#ifndef ISPN_HOTROD_TRANSPORT_TCPTRANSPORTFACTORY_H
#define ISPN_HOTROD_TRANSPORT_TCPTRANSPORTFACTORY_H



#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/sys/Mutex.h"

#include <vector>

namespace infinispan {
namespace hotrod {
namespace transport {

template <class K, class V> class PropsKeyedObjectPoolFactory;
template <class K, class V> class GenericKeyedObjectPool;
class RequestBalancingStrategy;
class InetSocketAddress;

class TcpTransportFactory : public TransportFactory
{
  public:
    void start(protocol::Codec& codec,
        const Configuration& configuration, int64_t topologyId);
    void destroy();

    Transport& getTransport();
    Transport& getTransport(const hrbytes& key);
    void releaseTransport(Transport& transport);
    void invalidateTransport(
        const InetSocketAddress& address, Transport* transport);

    bool isTcpNoDelay();
    int getTransportCount();
    int getSoTimeout();
    int getConnectTimeout();

    void updateServers(std::vector<InetSocketAddress>& );

  private:

    /**
     * We need mutex lock to atomically update fields of this class
     */
    infinispan::hotrod::sys::Mutex lock;
    std::vector<InetSocketAddress> servers;
    bool tcpNoDelay;
    int soTimeout;
    int connectTimeout;
    int transportCount;
    GenericKeyedObjectPool<InetSocketAddress, TcpTransport>* connectionPool;
    RequestBalancingStrategy* balancer;

    void createAndPreparePool(
        PropsKeyedObjectPoolFactory<InetSocketAddress, TcpTransport>& poolFactory);
    void updateTransportCount();
    void pingServers();
    Transport& borrowTransportFromPool(const InetSocketAddress& server);
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_TCPTRANSPORTFACTORY_H */

