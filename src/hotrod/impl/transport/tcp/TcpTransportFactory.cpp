#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"
#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"
#include "hotrod/impl/transport/tcp/RequestBalancingStrategy.h"
#include "hotrod/impl/protocol/Codec.h"
#include "hotrod/impl/configuration/Configuration.h"
#include <algorithm>

namespace infinispan {
namespace hotrod {

using protocol::Codec;
using namespace sys;

namespace transport {

TransportFactory* TransportFactory::newInstance() {
    return new TcpTransportFactory();
}

void TcpTransportFactory::start(
    Codec& codec, const Configuration& configuration, int64_t topologyId)
{
    ScopedLock<Mutex> l(lock);
    bool pingOnStartup = configuration.isPingOnStartup();
    for (std::vector<ServerConfiguration>::const_iterator iter=configuration.getServersConfiguration().begin();
        iter!=configuration.getServersConfiguration().end(); iter++)
    {
        servers.push_back(InetSocketAddress(iter->getHost(), iter->getPort()));
    }

    balancer.reset(RequestBalancingStrategy::newInstance());
    tcpNoDelay = configuration.isTcpNoDelay();
    soTimeout = configuration.getSocketTimeout();
    connectTimeout = configuration.getConnectionTimeout();

    // TODO: SSL configuration

    transportFactory.reset(new TransportObjectFactory(codec, *this, topologyId, pingOnStartup));

    createAndPreparePool(configuration.getConnectionPoolConfiguration());
    balancer->setServers(servers);
    updateTransportCount();

    if (configuration.isPingOnStartup()) {
       pingServers();
    }
 }

Transport& TcpTransportFactory::getTransport() {
	const InetSocketAddress* server = NULL;
    {
        ScopedLock<Mutex> l(lock);
        server = &balancer->nextServer();
    }
    return borrowTransportFromPool(*server);
}

Transport& TcpTransportFactory::getTransport(const hrbytes& /*key*/) {
    // TODO: consistent hash
    return getTransport();
}

void TcpTransportFactory::releaseTransport(Transport& transport) {
    ConnectionPool* pool = getConnectionPool();
    TcpTransport& tcpTransport = dynamic_cast<TcpTransport&>(transport);
    if (!tcpTransport.isValid()) {
    	pool->invalidateObject(tcpTransport.getServerAddress(), &tcpTransport);
    } else {
        pool->returnObject(tcpTransport.getServerAddress(), tcpTransport);
    }
}

void TcpTransportFactory::invalidateTransport(
    const InetSocketAddress& serverAddress, Transport* transport)
{
    ConnectionPool* pool = getConnectionPool();
    pool->invalidateObject(serverAddress, dynamic_cast<TcpTransport*>(transport));
}

bool TcpTransportFactory::isTcpNoDelay() {
    ScopedLock<Mutex> l(lock);
    return tcpNoDelay;
}

int TcpTransportFactory::getTransportCount() {
    ScopedLock<Mutex> l(lock);
    return transportCount;
}

int TcpTransportFactory::getSoTimeout() {
    ScopedLock<Mutex> l(lock);
    return soTimeout;
}

int TcpTransportFactory::getConnectTimeout() {
    ScopedLock<Mutex> l(lock);
    return connectTimeout;
}

void TcpTransportFactory::createAndPreparePool(const ConnectionPoolConfiguration& configuration)
{
    connectionPool.reset(new ConnectionPool(transportFactory, configuration));
    for (std::vector<InetSocketAddress>::const_iterator i = servers.begin();
        i != servers.end() ; ++i)
    {
        connectionPool->preparePool(*i);
    }
}

void TcpTransportFactory::pingServers() {
    for (std::vector<InetSocketAddress>::const_iterator iter = servers.begin();
        iter != servers.end(); iter++) {
       try {
          // Go through all statically configured nodes and force a
          // connection to be established and a ping message to be sent.
          connectionPool->returnObject(*iter, connectionPool->borrowObject(*iter));
       } catch (Exception& e) {
          // Ping's objective is to retrieve a potentially newer
          // version of the Hot Rod cluster topology, so ignore
          // exceptions from nodes that might not be up any more.
       }
    }
}

void TcpTransportFactory::updateTransportCount() {
    ScopedLock<Mutex> l(lock);
    int maxActive = connectionPool->getConfiguration().getMaxActive();
    int size = servers.size();
    if (maxActive > 0) {
        transportCount = (maxActive * size > maxActive) ?
            maxActive * size : maxActive;
    } else {
        transportCount = 10 * size;
    }
}

void TcpTransportFactory::destroy() {
    ScopedLock<Mutex> l(lock);
    connectionPool->clear();
    connectionPool->close();
}

Transport& TcpTransportFactory::borrowTransportFromPool(
    const InetSocketAddress& server)
{
    ConnectionPool* pool = getConnectionPool();
    return pool->borrowObject(server);
}

ConnectionPool* TcpTransportFactory::getConnectionPool()
{
    ScopedLock<Mutex> l(lock);
    return connectionPool.get();
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
