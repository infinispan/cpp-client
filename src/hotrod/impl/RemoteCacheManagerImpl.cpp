#include <hotrod/impl/NearRemoteCacheImpl.h>
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
        listenerNotifier.reset(ClientListenerNotifier::create(transportFactory));
        transportFactory->start(*codec, defaultCacheTopologyId, listenerNotifier.get());

       for(std::map<std::string, RemoteCacheHolder>::iterator iter = cacheName2RemoteCache.begin(); iter != cacheName2RemoteCache.end(); ++iter ) {
           startRemoteCache(*iter->second.first.get(), iter->second.second);
       }

        started = true;
	}
}

void RemoteCacheManagerImpl::stop() {
	ScopedLock<Mutex> l(lock);
	if (started) {
        if  (listenerNotifier)
            listenerNotifier->stop();
        transportFactory->destroy();
        started = false;
    }
}

bool RemoteCacheManagerImpl::isStarted() {
	ScopedLock<Mutex> l(lock);
    return started;
}

const Configuration& RemoteCacheManagerImpl::getConfiguration() {
    return configuration;
}

std::shared_ptr<RemoteCacheImpl> RemoteCacheManagerImpl::createRemoteCache(
    bool forceReturnValue, NearCacheConfiguration nc)
{
    return createRemoteCache(DefaultCacheName,forceReturnValue, nc);
}

std::shared_ptr<RemoteCacheImpl> RemoteCacheManagerImpl::createRemoteCache(
    const std::string& name, bool forceReturnValue, NearCacheConfiguration nc)
{
    ScopedLock<Mutex> l(lock);
    std::string nameAndForceFlag = (forceReturnValue) ? name+"/true" : name+"/false";
    std::map<std::string, RemoteCacheHolder>::iterator iter = cacheName2RemoteCache.find(nameAndForceFlag);
    // Cache found
    if (iter != cacheName2RemoteCache.end()) {
        return iter->second.first;
    }
    // Cache not found, creating...
    std::shared_ptr<RemoteCacheImpl> rcache_sptr;
    if (nc.getMode()==NearCacheMode::DISABLED)
    {
        rcache_sptr.reset(new RemoteCacheImpl(*this, name));
    }
    else
    {
    	rcache_sptr.reset(new NearRemoteCacheImpl(*this, name, nc));
    }
    try {
        startRemoteCache(*rcache_sptr, forceReturnValue);
        if (name != DefaultCacheName) {
           // If ping not successful assume that the cache does not exist
           // Default cache is always started, so don't do for it
           if (ping(*rcache_sptr) == CACHE_DOES_NOT_EXIST) {
               return nullptr;
           }
        }
        // If ping on startup is disabled, or cache is defined in server
        cacheName2RemoteCache[nameAndForceFlag] = RemoteCacheHolder(rcache_sptr, forceReturnValue);
        return rcache_sptr;
    }
    catch (...)
    {
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

bool RemoteCacheManagerImpl::clusterSwitch(std::string clusterName)
{
	return transportFactory->clusterSwitch(clusterName);
}

bool RemoteCacheManagerImpl::clusterSwitch()
{
	return transportFactory->clusterSwitch();
}

}} // namespace infinispan::hotrod
