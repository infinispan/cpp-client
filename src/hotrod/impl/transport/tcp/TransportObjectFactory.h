#ifndef ISPN_HOTROD_TRANSPORT_TRANSPORTOBJECTFACTORY_H
#define ISPN_HOTROD_TRANSPORT_TRANSPORTOBJECTFACTORY_H

#include "hotrod/impl/transport/tcp/InetSocketAddress.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/operations/PingOperation.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class TcpTransportFactory;

class TransportObjectFactory
{
  public:
    TransportObjectFactory(
        protocol::Codec& codec,
        TcpTransportFactory& tcpTransportFactory,
        int64_t topologyId, bool pingOnStartup);

    TcpTransport& makeObject(const InetSocketAddress& address);
    bool validateObject(const InetSocketAddress& address, TcpTransport& transport);
    void destroyObject(const InetSocketAddress& address, TcpTransport& transport);

    void activateObject(const InetSocketAddress& address, TcpTransport& transport);
    void passivateObject(const InetSocketAddress& address, TcpTransport& transport);

  private:
    TcpTransportFactory& tcpTransportFactory;
    int64_t topologyId;
    bool pingOnStartup;
    infinispan::hotrod::protocol::Codec& codec;

    bool firstPingExecuted;

    operations::PingResult ping(TcpTransport& tcpTransport, int64_t topologyId);
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_TRANSPORTOBJECTFACTORY_H */
