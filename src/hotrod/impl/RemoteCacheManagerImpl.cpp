#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "hotrod/impl/RemoteCacheManagerImpl.h"
#include "hotrod/impl/RemoteCacheImpl.h"
#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/operations/OperationsFactory.h"

namespace infinispan {
namespace hotrod {

using namespace protocol;
using namespace transport;
using namespace operations;
using namespace sys;

const std::string ISPN_CLIENT_HOTROD_SERVER_LIST("infinispan.client.hotrod.server_list");

const std::string DefaultCacheName = "";

RemoteCacheManagerImpl::RemoteCacheManagerImpl(bool start_)
  : started(false),
    configuration(ConfigurationBuilder().build()), codec(0),
	defaultCacheTopologyId(protocol::HotRodConstants::DEFAULT_CACHE_TOPOLOGY)
{
	; //force topology read on first op
	if (start_) start();
}

Configuration buildConfig(const std::map<std::string,std::string>& properties)
{
  std::map<std::string,std::string>::const_iterator server_prop;
  server_prop = properties.find(ISPN_CLIENT_HOTROD_SERVER_LIST);
  if(server_prop != properties.end()) {
      std::string serverList = server_prop->second;
      return ConfigurationBuilder().addServers(serverList).build();
  }
  return ConfigurationBuilder().build();
}

RemoteCacheManagerImpl::RemoteCacheManagerImpl(const std::map<std::string,std::string>& properties, bool start_)
  : started(false),
    configuration(buildConfig(properties)), codec(0),
	defaultCacheTopologyId(protocol::HotRodConstants::DEFAULT_CACHE_TOPOLOGY)
{
  if (start_) start();
}



RemoteCacheManagerImpl::RemoteCacheManagerImpl(const Configuration& configuration_, bool start_)
  : started(false),
    configuration(configuration_), codec(0),
	defaultCacheTopologyId(protocol::HotRodConstants::DEFAULT_CACHE_TOPOLOGY)
{
  if (start_) start();
}

void RemoteCacheManagerImpl::start() {
    ScopedLock<Mutex> l(lock);
    codec = CodecFactory::getCodec(configuration.getProtocolVersionCString());
    if (!started) {
        transportFactory.reset(TransportFactory::newInstance(configuration));
        transportFactory->start(*codec, defaultCacheTopologyId);
        listenerNotifier = ClientListenerNotifier::create();
       for(std::map<std::string, RemoteCacheHolder>::iterator iter = cacheName2RemoteCache.begin(); iter != cacheName2RemoteCache.end(); ++iter ) {
           startRemoteCache(*iter->second.first.get(), iter->second.second);
       }

        started = true;
	}
}

void RemoteCacheManagerImpl::stop() {
	ScopedLock<Mutex> l(lock);
	if (started) {
        transportFactory->destroy();
        started = false;
        delete listenerNotifier;
    }
}

bool RemoteCacheManagerImpl::isStarted() {
	ScopedLock<Mutex> l(lock);
    return started;
}

const Configuration& RemoteCacheManagerImpl::getConfiguration() {
    return configuration;
}

RemoteCacheImpl *RemoteCacheManagerImpl::createRemoteCache(
    bool forceReturnValue)
{
    return createRemoteCache(DefaultCacheName,forceReturnValue);
}

RemoteCacheImpl *RemoteCacheManagerImpl::createRemoteCache(
    const std::string& name, bool forceReturnValue)
{
    ScopedLock<Mutex> l(lock);
    std::map<std::string, RemoteCacheHolder>::iterator iter = cacheName2RemoteCache.find(name);
    // Cache found
    if (iter != cacheName2RemoteCache.end()) {
        return iter->second.first.get();
    }
    // Cache not found, creating...
    RemoteCacheImpl *rcache = new RemoteCacheImpl(*this, name);
    try {
        startRemoteCache(*rcache, forceReturnValue);
        if (name != DefaultCacheName) {
           // If ping not successful assume that the cache does not exist
           // Default cache is always started, so don't do for it
           if (ping(*rcache) == CACHE_DOES_NOT_EXIST) {
               delete rcache;
               return NULL;
           }
        }
        // If ping on startup is disabled, or cache is defined in server
        cacheName2RemoteCache[name] = RemoteCacheHolder(rcache, forceReturnValue);
        return rcache;
    }
    catch (...)
    {
        std::cout << "Deleting rcache" << std::endl;
        delete rcache;
        throw;
    }
}

void RemoteCacheManagerImpl::startRemoteCache(RemoteCacheImpl& remoteCache, bool forceReturnValue)
{
    OperationsFactory* operationsFactory = new OperationsFactory(
        transportFactory,
        remoteCache.getName(),
        forceReturnValue,
        *CodecFactory::getCodec(configuration.getProtocolVersionCString()));

    remoteCache.init(operationsFactory);

}

PingResult RemoteCacheManagerImpl::ping(RemoteCacheImpl& remoteCache) {
    if (!transportFactory) {
        return FAIL;
    }

    return remoteCache.ping();
}

ClusterStatus RemoteCacheManagerImpl::clusterSwitch(std::string clusterName)
{
	return transportFactory->clusterSwitch(clusterName);
}

ClusterStatus RemoteCacheManagerImpl::clusterSwitch()
{
	return transportFactory->clusterSwitch();
}

}} // namespace infinispan::hotrod
