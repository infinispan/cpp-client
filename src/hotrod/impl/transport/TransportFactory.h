#ifndef ISPN_HOTROD_TRANSPORT_TRANSPORTFACTORY_H
#define ISPN_HOTROD_TRANSPORT_TRANSPORTFACTORY_H



#include "hotrod/impl/transport/Transport.h"
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

namespace transport {

class Transport;
class InetSocketAddress;

class TransportFactory
{
  public:
    virtual void start(protocol::Codec& codec,
        const Configuration& configuration, int64_t topologyId) = 0;
    virtual void destroy() = 0;

    virtual Transport& getTransport() = 0;
    virtual Transport& getTransport(const hrbytes& key) = 0;
    virtual void releaseTransport(Transport& transport) = 0;
    virtual void invalidateTransport(
        const InetSocketAddress& serverAddress, Transport* transport) = 0;

    virtual bool isTcpNoDelay() = 0;
    virtual int getTransportCount() = 0;
    virtual int getSoTimeout() = 0;
    virtual int getConnectTimeout() = 0;

    virtual void updateServers(std::vector<InetSocketAddress>& ) = 0;
    virtual ~TransportFactory() {}

    // TODO: consistent hash
    // void updateServers(const std::vector<InetSocketAddress>& )
    // updateHashFunction
    // getConsistentHashFactory
    // TODO: ssl
    // getSSLContext

  private:
    static TransportFactory* newInstance();

  friend class infinispan::hotrod::RemoteCacheManagerImpl;
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_TRANSPORTFACTORY_H */

