

#include "hotrod/impl/RemoteCacheManagerImpl.h"
#include "hotrod/impl/RemoteCacheImpl.h"
#include "hotrod/impl/configuration/ConfigurationBuilder.h"
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
  : started(false), topologyId(0),
    configuration(ConfigurationBuilder().build()), codec(0)
{
	if (start_) start();
}

RemoteCacheManagerImpl::RemoteCacheManagerImpl(const std::map<std::string,std::string>& properties, bool start_)
  : started(false), topologyId(0),
    configuration(ConfigurationBuilder().build()), codec(0)
{
  std::map<std::string,std::string>::const_iterator server_prop;

  server_prop = properties.find(ISPN_CLIENT_HOTROD_SERVER_LIST);
  if(server_prop != properties.end()) {
      std::string serverList = server_prop->second;
      configuration = ConfigurationBuilder().addServers(serverList).build();
  }

  if (start_) start();
}

void RemoteCacheManagerImpl::start() {
    ScopedLock<Mutex> l(lock);
    codec = CodecFactory::getCodec(configuration.getProtocolVersion().c_str());
    if (!started) {
        transportFactory.reset(TransportFactory::newInstance());
        transportFactory->start(*codec, configuration, topologyId);

       for(std::map<std::string, RemoteCacheHolder>::iterator iter = cacheName2RemoteCache.begin();
        		iter != cacheName2RemoteCache.end(); ++iter )
        {
        	startRemoteCache(*iter->second.first, iter->second.second);
        }

        started = true;
	}
}

void RemoteCacheManagerImpl::stop() {
	ScopedLock<Mutex> l(lock);
	if (started) {
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

HR_SHARED_PTR<RemoteCacheImpl> RemoteCacheManagerImpl::createRemoteCache(
    bool forceReturnValue)
{
    return createRemoteCache(DefaultCacheName,forceReturnValue);
}

HR_SHARED_PTR<RemoteCacheImpl> RemoteCacheManagerImpl::createRemoteCache(
    const std::string& name, bool forceReturnValue)
{
    ScopedLock<Mutex> l(lock);
    std::map<std::string, RemoteCacheHolder>::iterator iter = cacheName2RemoteCache.find(name);
    if (iter == cacheName2RemoteCache.end()) {
        HR_SHARED_PTR<RemoteCacheImpl> rcache(new RemoteCacheImpl(*this,name));
        startRemoteCache(*rcache, forceReturnValue);
        if (configuration.isPingOnStartup()) {
            // If ping not successful assume that the cache does not exist
            // Default cache is always started, so don't do for it
            if (rcache->getName() != DefaultCacheName && ping(*rcache) == CACHE_DOES_NOT_EXIST) {
                rcache.reset();
                return rcache;
            }
        }
        // If ping on startup is disabled, or cache is defined in server
        cacheName2RemoteCache[name] = RemoteCacheHolder(rcache, forceReturnValue);
        return rcache;
    }

    return iter->second.first;
}

void RemoteCacheManagerImpl::startRemoteCache(RemoteCacheImpl& remoteCache, bool forceReturnValue)
{
    OperationsFactory* operationsFactory = new OperationsFactory(
        transportFactory,
        remoteCache.getName(),
        topologyId,
        forceReturnValue,
        *CodecFactory::getCodec(configuration.getProtocolVersion().c_str()));

    remoteCache.init(operationsFactory);

}

PingResult RemoteCacheManagerImpl::ping(RemoteCacheImpl& remoteCache) {
    if (!transportFactory) {
        return FAIL;
    }

    return remoteCache.ping();
}

}} // namespace infinispan::hotrod
