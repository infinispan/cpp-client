

#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"
#include "hotrod/impl/transport/tcp/PropsKeyedObjectPoolFactory.h"
#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"
#include "hotrod/impl/transport/tcp/RequestBalancingStrategy.h"
#include "hotrod/impl/protocol/Codec.h"
#include "hotrod/impl/configuration/Configuration.h"
#include <algorithm>

namespace infinispan {
namespace hotrod {

using protocol::Codec;
using infinispan::hotrod::sys::ScopedLock;
using infinispan::hotrod::sys::Mutex;

namespace transport {

TransportFactory* TransportFactory::newInstance() {
    return new TcpTransportFactory();
}

void TcpTransportFactory::start(
    Codec& codec, const Configuration& configuration, int64_t topologyId)
{
    // TODO: multithread (lock)
    // TODO: consistent hash
    bool pingOnStartup = configuration.isPingOnStartup();
    for (std::vector<ServerConfiguration>::const_iterator iter=configuration.getServersConfiguration().begin();
        iter!=configuration.getServersConfiguration().end(); iter++)
    {
        servers.push_back(InetSocketAddress(iter->getHost(), iter->getPort()));
    }

    balancer = RequestBalancingStrategy::newInstance();
    tcpNoDelay = configuration.isTcpNoDelay();
    soTimeout = configuration.getSocketTimeout();
    connectTimeout = configuration.getConnectionTimeout();

    // TODO: SSL configuration

    PropsKeyedObjectPoolFactory<InetSocketAddress, TcpTransport> poolFactory(
        *new TransportObjectFactory(codec, *this, topologyId, pingOnStartup),
        configuration.getConnectionPoolConfiguration());

    createAndPreparePool(poolFactory);
    balancer->setServers(servers);
    updateTransportCount();

    if (configuration.isPingOnStartup()) {
       pingServers();
    }
 }

Transport& TcpTransportFactory::getTransport() {
    const InetSocketAddress& server = balancer->nextServer();
    return borrowTransportFromPool(server);
}

Transport& TcpTransportFactory::getTransport(const hrbytes& /*key*/) {
    // TODO: consistent hash
    return getTransport();
}

void TcpTransportFactory::releaseTransport(Transport& transport) {
    TcpTransport& tcpTransport = dynamic_cast<TcpTransport&>(transport);
    if (!tcpTransport.isValid()) {
      connectionPool->invalidateObject(tcpTransport.getServerAddress(), &tcpTransport);
    } else {
        connectionPool->returnObject(tcpTransport.getServerAddress(), tcpTransport);
    }
}

void TcpTransportFactory::invalidateTransport(
    const InetSocketAddress& serverAddress, Transport* transport)
{
    connectionPool->invalidateObject(
        serverAddress, dynamic_cast<TcpTransport*>(transport));
}

bool TcpTransportFactory::isTcpNoDelay() {
    return tcpNoDelay;
}

int TcpTransportFactory::getTransportCount() {
    return transportCount;
}

int TcpTransportFactory::getSoTimeout() {
    return soTimeout;
}

int TcpTransportFactory::getConnectTimeout() {
    return connectTimeout;
}

void TcpTransportFactory::createAndPreparePool(
    PropsKeyedObjectPoolFactory<InetSocketAddress, TcpTransport>& poolFactory)
{
    connectionPool = poolFactory.createPool();
    for (std::vector<InetSocketAddress>::const_iterator i = servers.begin();
        i != servers.end() ; ++i)
    {
        connectionPool->preparePool(*i);
    }
}

void TcpTransportFactory::pingServers() {
    for (std::vector<InetSocketAddress>::const_iterator iter = servers.begin();
        iter != servers.end(); iter++) {
      InetSocketAddress addr = *iter;
       try {
          // Go through all statically configured nodes and force a
          // connection to be established and a ping message to be sent.
          connectionPool->returnObject(addr, connectionPool->borrowObject(addr));
       } catch (Exception& e) {
          // Ping's objective is to retrieve a potentially newer
          // version of the Hot Rod cluster topology, so ignore
          // exceptions from nodes that might not be up any more.
       }
    }
}

void TcpTransportFactory::updateTransportCount() {
    unsigned int maxActive = connectionPool->getMaxActive();
    if (maxActive > 0) {
        transportCount = (maxActive * servers.size() > maxActive) ?
            maxActive * servers.size() : maxActive;
        //to avoid int overflow when maxActive is very high!
    } else {
        transportCount = 10 * servers.size();
    }
}

void TcpTransportFactory::destroy() {
  connectionPool->clear();
    connectionPool->close();
  // TODO: clean connection pool
  /*
    try {
       connectionPool->close();
    } catch (Exception e) {
       log.warn("Exception while shutting down the connection pool.", e);
    }
    */
  delete connectionPool;
  connectionPool = NULL;
  delete balancer;
  balancer = NULL;
}

Transport& TcpTransportFactory::borrowTransportFromPool(
    const InetSocketAddress& server)
{
    // TODO
    return connectionPool->borrowObject(server);
}

void TcpTransportFactory::updateServers(std::vector<InetSocketAddress>& newServers) {

    ScopedLock<Mutex> l(lock);
    std::vector<InetSocketAddress> addedServers;
    std::sort(newServers.begin(), newServers.end());
    std::sort(servers.begin(), servers.end());

    std::set_difference(newServers.begin(), newServers.end(), servers.begin(),
            servers.end(), std::inserter(addedServers, addedServers.end()));

    std::vector<InetSocketAddress> failedServers;
    std::sort(newServers.begin(), newServers.end());
    std::sort(servers.begin(), servers.end());

    std::set_difference(servers.begin(), servers.end(), newServers.begin(),
            newServers.end(), std::inserter(failedServers, failedServers.end()));

    if (failedServers.empty() && newServers.empty()) {
        return;
    }

    //1. first add new servers. For servers that went down, the returned transport will fail for now
    for (std::vector<InetSocketAddress>::const_iterator it =
            addedServers.begin(); it != addedServers.end(); ++it) {
        connectionPool->addObject(*it);
    }

    //2. now set the server list to the active list of servers. All the active servers (potentially together with some
    // failed servers) are in the pool now. But after this, the pool won't be asked for connections to failed servers,
    // as the balancer will only know about the active servers
    balancer->setServers(newServers);

    //3. Now just remove failed servers
    for (std::vector<InetSocketAddress>::const_iterator it =
            failedServers.begin(); it != failedServers.end(); ++it) {
        connectionPool->clear(*it);
    }

    servers.clear();
    servers = newServers;
    updateTransportCount();
}

}}} // namespace infinispan::hotrod::transport
