

#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"
#include "hotrod/impl/transport/tcp/PropsKeyedObjectPoolFactory.h"
#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"
#include "hotrod/impl/transport/tcp/RequestBalancingStrategy.h"
#include "hotrod/impl/protocol/Codec.h"
#include "hotrod/impl/configuration/Configuration.h"


namespace infinispan {
namespace hotrod {
namespace transport {

using protocol::Codec;
using namespace sys;

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

    PropsKeyedObjectPoolFactory<InetSocketAddress, TcpTransport>* poolFactory =
        new PropsKeyedObjectPoolFactory<InetSocketAddress, TcpTransport>(
            new TransportObjectFactory(codec, *this, topologyId, pingOnStartup),
            configuration.getConnectionPoolConfiguration());

    createAndPreparePool(poolFactory);
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
	GenericKeyedObjectPool<InetSocketAddress, TcpTransport>* pool = getConnectionPool();
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
	GenericKeyedObjectPool<InetSocketAddress, TcpTransport>* pool = getConnectionPool();
    pool->invalidateObject(
        serverAddress, dynamic_cast<TcpTransport*>(transport));
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

void TcpTransportFactory::createAndPreparePool(
    PropsKeyedObjectPoolFactory<InetSocketAddress, TcpTransport>* poolFactory)
{
    connectionPool.reset(poolFactory->createPool());
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
    ScopedLock<Mutex> l(lock);
    int64_t maxActive = connectionPool->getMaxActive();
    if (maxActive > 0) {
        transportCount = maxActive * servers.size();
        // TODO: in java code avoid int overflow when maxActive is very high!
    } else {
        transportCount = 10 * servers.size();
    }
}

void TcpTransportFactory::destroy() {
    ScopedLock<Mutex> l(lock);
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
}

Transport& TcpTransportFactory::borrowTransportFromPool(
    const InetSocketAddress& server)
{
	GenericKeyedObjectPool<InetSocketAddress, TcpTransport>* pool = getConnectionPool();
    return pool->borrowObject(server);
}

GenericKeyedObjectPool<InetSocketAddress, TcpTransport>* TcpTransportFactory::getConnectionPool()
{
    ScopedLock<Mutex> l(lock);
    return connectionPool.get();
}

}}} // namespace infinispan::hotrod::transport
