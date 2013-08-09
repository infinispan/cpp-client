

#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"

namespace infinispan {
namespace hotrod {

using protocol::Codec;
using namespace operations;

namespace transport {


TransportObjectFactory::TransportObjectFactory(
    Codec& c, TcpTransportFactory& factory, int64_t tid, bool pingable)
    : tcpTransportFactory(factory), topologyId(tid),
      pingOnStartup(pingable), codec(c), firstPingExecuted(false)
{ }

TcpTransport& TransportObjectFactory::makeObject(const InetSocketAddress& address) {
    TcpTransport* tcpTransport = new TcpTransport(address, tcpTransportFactory);
    if (pingOnStartup && !firstPingExecuted) {
        firstPingExecuted = true;

        // Don't ignore exceptions from ping() command, since
        // they indicate that the transport instance is invalid.
        ping(*tcpTransport, topologyId);
    }
    return *tcpTransport;
}

bool TransportObjectFactory::validateObject(const InetSocketAddress& /*address*/, TcpTransport& transport) {
    return ping(transport, topologyId) == SUCCESS;
}

void TransportObjectFactory::destroyObject(const InetSocketAddress& /*address*/, TcpTransport& transport) {
    delete &transport;
}

void TransportObjectFactory::activateObject(const InetSocketAddress& /*address*/, TcpTransport& /*transport*/) {
    // empty
}

void TransportObjectFactory::passivateObject(
    const InetSocketAddress& /*address*/, TcpTransport& /*transport*/)
{
  // empty
}

PingResult TransportObjectFactory::ping(
    TcpTransport& tcpTransport, int64_t _topologyId)
{
    PingOperation po(codec, _topologyId, tcpTransport);
    return po.execute();
}

}}} // namespace infinispan::hotrod::transport
