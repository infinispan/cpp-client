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
namespace consistenthash{
class ConsistentHashFactory;
}

namespace transport {

class Transport;
class InetSocketAddress;

class TransportFactory
{
  public:
    virtual void start(protocol::Codec& codec) = 0;
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

    virtual void updateHashFunction(
            std::map<InetSocketAddress, std::set<int32_t> >& servers2Hash,
            int32_t numKeyOwners, uint8_t hashFunctionVersion,
            int32_t hashSpace) = 0;
    virtual void clearHashFunction() = 0;

    virtual infinispan::hotrod::consistenthash::ConsistentHashFactory
    & getConsistentHashFactory() = 0;

    // TODO: ssl
    // getSSLContext

  private:
    static TransportFactory* newInstance(const Configuration& config);

  friend class infinispan::hotrod::RemoteCacheManagerImpl;
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_TRANSPORTFACTORY_H */

