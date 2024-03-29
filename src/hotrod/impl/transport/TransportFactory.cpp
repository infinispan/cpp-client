#include <infinispan/hotrod/exceptions.h>
#include <infinispan/hotrod/InetSocketAddress.h>
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransport.h"
#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"
#include "hotrod/impl/transport/tcp/RoundRobinBalancingStrategy.h"
#include "hotrod/impl/protocol/Codec.h"
#include "infinispan/hotrod/Configuration.h"
#include "hotrod/impl/consistenthash/ConsistentHash.h"
#include "hotrod/sys/Log.h"
#include <algorithm>

namespace infinispan {
namespace hotrod {

using protocol::Codec;
using namespace sys;
using namespace consistenthash;

namespace transport {

TransportFactory* TransportFactory::newInstance(const Configuration& configuration) {
    return new TransportFactory(configuration);
}

void TransportFactory::start(
    Codec& codec, ClientListenerNotifier* listenerNotifier)
{
	ScopedLock<Mutex> l(lock);
	topologyAge = 0;
    transportFactory.reset(new TransportObjectFactory(codec, *this));
    auto serversMap = configuration.getServersMapConfiguration();
    std::vector<ServerConfiguration>* configuredServers;
    if (serversMap.find(Configuration::DEFAULT_CLUSTER_NAME)!=serversMap.end())
    {
    	configuredServers = &serversMap[Configuration::DEFAULT_CLUSTER_NAME];
    	currCluster=Configuration::DEFAULT_CLUSTER_NAME;
    }
    else
    {
    	configuredServers = &serversMap.begin()->second;
    	currCluster=serversMap.begin()->first;
    }
    for (std::vector<ServerConfiguration>::const_iterator iter = configuredServers->begin();
        iter != configuredServers->end(); iter++)
    {
        initialServers.push_back(InetSocketAddress(iter->getHostCString(), iter->getPort()));
    }

    FailOverRequestBalancingStrategy::ProducerFn producerFn=configuration.getBalancingStrategy();
    if (producerFn!= nullptr) {
        balancer.reset((*producerFn)());
    }
    else
    {
        balancer.reset(RoundRobinBalancingStrategy::newInstance());
    }

    createAndPreparePool();

    balancer->setServers(initialServers);
    sniHostName = configuration.getSslConfiguration().sniHostName();
    try
    {
        pingServers();
    }
    catch (Exception &e) {
        throw e;
    }
    this->listenerNotifier = listenerNotifier;
 }

std::vector<ServerConfiguration> TransportFactory::getNextWorkingServersConfiguration() {
	for (auto p: configuration.getServersMapConfiguration()){
		if (p.first==currCluster)
			continue;
		for (auto v: p.second)
		{
			try
			{
				pingExternalServer(InetSocketAddress(v.getHost(), v.getPort()));
				currCluster=p.first;
				return p.second;
			}
			catch (Exception &)
			{
			}
		}
	}
	return std::vector<ServerConfiguration>();
}

transport::Transport& TransportFactory::getTransport(const std::vector<char>& /*cacheName*/, const std::set<transport::InetSocketAddress>& failedServers) {
    const InetSocketAddress* server = &balancer->nextServer(failedServers);
    return borrowTransportFromPool(*server);
}

transport::Transport& TransportFactory::getTransport(const std::vector<char>& key, const std::vector<char>& cacheName, const std::set<transport::InetSocketAddress>& failedServers) {
    InetSocketAddress server;
    {
        server = topologyInfo.getHashAwareServer(key,cacheName);
        if (server.isEmpty())
        {   // Return balanced transport
        	return getTransport(cacheName, failedServers);
        }
        return borrowTransportFromPool(server);
    }
}

void TransportFactory::releaseTransport(Transport& transport) {
    ConnectionPool* pool = getConnectionPool();
    TcpTransport& tcpTransport = dynamic_cast<TcpTransport&>(transport);
    if (!tcpTransport.isValid()) {
        pool->invalidateObject(tcpTransport.getServerAddress(), &tcpTransport);
    } else {
        pool->returnObject(tcpTransport.getServerAddress(), tcpTransport);
    }
}

void TransportFactory::invalidateTransport(
    const InetSocketAddress& serverAddress, Transport* transport)
{
    ConnectionPool* pool = getConnectionPool();
    pool->invalidateObject(serverAddress, dynamic_cast<TcpTransport*>(transport));
}

bool TransportFactory::clusterSwitch()
{
    auto configuredServers = getNextWorkingServersConfiguration();
	if (configuredServers.size()==0)
	{
		return false;
	}
	connectionPool->close();
	ScopedLock<Mutex> l(lock);
	topologyAge = 0;
    initialServers.clear();
    failedServers.clear();
    for (auto iter = configuredServers.begin();
        iter != configuredServers.end(); iter++)
    {
        initialServers.push_back(InetSocketAddress(iter->getHostCString(), iter->getPort()));
    }

    auto producerFn=configuration.getBalancingStrategy();
    if (producerFn!= nullptr) {
        balancer.reset((*producerFn)());
    }
    else
    {
        balancer.reset(RoundRobinBalancingStrategy::newInstance());
    }
    topologyInfo.updateServers(initialServers);

    createAndPreparePool();

    balancer->setServers(initialServers);
    pingServers();
    return true;
}

bool TransportFactory::clusterSwitch(std::string clusterName)
{
	auto servers=configuration.getServersMapConfiguration();
	if (servers.find(clusterName)==servers.end())
		return false;
    auto configuredServers = servers[clusterName];
	ScopedLock<Mutex> l(lock);
	topologyAge = 0;
    initialServers.clear();
    failedServers.clear();
    for (auto iter = configuredServers.begin();
        iter != configuredServers.end(); iter++)
    {
        initialServers.push_back(InetSocketAddress(iter->getHostCString(), iter->getPort()));
    }

    auto producerFn=configuration.getBalancingStrategy();
    if (producerFn!= nullptr) {
        balancer.reset((*producerFn)());
    }
    else
    {
        balancer.reset(RoundRobinBalancingStrategy::newInstance());
    }
    // Consider all the current server as failed
    auto failedServers = topologyInfo.getServers();
    topologyInfo.updateServers(initialServers);

    createAndPreparePool();

    balancer->setServers(initialServers);
    pingServers();
    return true;
}

bool TransportFactory::isTcpNoDelay() {
    return configuration.isTcpNoDelay();
}

int TransportFactory::getMaxRetries() {
    return maxRetries;
}

int TransportFactory::getSoTimeout() {
    return configuration.getSocketTimeout();
}

int TransportFactory::getConnectTimeout() {
    return configuration.getConnectionTimeout();
}

bool TransportFactory::isSslEnabled() {
    return configuration.getSslConfiguration().enabled();
}

const std::string& TransportFactory::getSslServerCAPath() {
    return configuration.getSslConfiguration().serverCAPath();
}

const std::string& TransportFactory::getSslServerCAFile() {
    return configuration.getSslConfiguration().serverCAFile();
}

const std::string& TransportFactory::getSslClientCertificateFile() {
    return configuration.getSslConfiguration().clientCertificateFile();
}

void TransportFactory::createAndPreparePool()
{
    connectionPool.reset(new ConnectionPool(transportFactory, configuration.getConnectionPoolConfiguration()));
    std::vector<InetSocketAddress> servers = topologyInfo.getServers();
    for (std::vector<InetSocketAddress>::const_iterator i = servers.begin();
        i != servers.end() ; ++i)
    {
        connectionPool->preparePool(*i);
    }
}

void TransportFactory::pingExternalServer(InetSocketAddress s) {
	transport::TcpTransport& t = transportFactory->makeObject(s);
	transportFactory->ping(t);
	transportFactory->destroyObject(s, t);
}


void TransportFactory::pingServers() {
    std::vector<InetSocketAddress> s = topologyInfo.getServers();
    for (std::vector<InetSocketAddress>::const_iterator iter = s.begin(); iter != s.end(); iter++) {
        TcpTransport* transport = nullptr;
        try {
            // Go through all statically configured nodes and force a
            // connection to be established and a ping message to be sent.
            transport = &connectionPool->borrowObject(*iter);
            transportFactory->ping(*transport);
        } catch (const Exception &e) {
            ERROR("Initial ping has thrown an exception when pinging %s:%d : %s",
                iter->getHostname().c_str(), iter->getPort(), e.what());
            // Ping's objective is to retrieve a potentially newer
            // version of the Hot Rod cluster topology, so ignore
            // exceptions from nodes that might not be up any more.
        }
        if (transport != nullptr)
        {
            connectionPool->returnObject(*iter, *transport);
        }
    }
}

void TransportFactory::destroy() {
    ScopedLock<Mutex> l(lock);
    connectionPool->clear();
    connectionPool->close();
}

Transport& TransportFactory::borrowTransportFromPool(
    const InetSocketAddress& server)
{
    ConnectionPool* pool = getConnectionPool();
    return pool->borrowObject(server);
}

ConnectionPool* TransportFactory::getConnectionPool()
{
    return connectionPool.get();
}

void TransportFactory::updateServers(std::vector<InetSocketAddress>& newServers) {
    ScopedLock<Mutex> l(lock);
    std::vector<InetSocketAddress> addedServers;
    std::vector<InetSocketAddress> topoServers=topologyInfo.getServers();

    std::sort(newServers.begin(), newServers.end());

    std::sort(topoServers.begin(), topoServers.end());

    std::set_difference(newServers.begin(), newServers.end(), topoServers.begin(),
    		topoServers.end(), std::inserter(addedServers, addedServers.end()));

    std::vector<InetSocketAddress> failedServers;
    std::sort(newServers.begin(), newServers.end());
    std::sort(topoServers.begin(), topoServers.end());

    std::set_difference(topoServers.begin(), topoServers.end(), newServers.begin(),
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

    topoServers.clear();
    topologyInfo.updateServers(newServers);
}

void TransportFactory::updateHashFunction(
		std::vector<std::vector<InetSocketAddress>>& segmentOwners,
        uint32_t &numSegment, uint8_t &hashFunctionVersion,
        const std::vector<char>& cacheName, int topologyId)
{
    ScopedLock<Mutex> l(lock);
    TRACE("TransportFactory::updateHashFunction(): hashversion=%d, topologyId=%d",hashFunctionVersion, topologyId);
    topologyInfo.updateTopology(segmentOwners, numSegment, hashFunctionVersion, cacheName, topologyId);
}

const std::string& TransportFactory::getSniHostName(){
    return sniHostName;

}

}}} // namespace infinispan::hotrod::transport
