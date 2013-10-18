#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "hotrod/impl/RemoteCacheManagerImpl.h"
#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/operations/OperationsFactory.h"

namespace infinispan {
namespace hotrod {


using namespace protocol;
using namespace transport;
using namespace operations;


RemoteCacheManager::RemoteCacheManager(bool start_)
  : Handle<RemoteCacheManagerImpl>(new RemoteCacheManagerImpl(start_)) { }

RemoteCacheManager::RemoteCacheManager(const std::map<std::string,std::string>& properties, bool start_)
  : Handle<RemoteCacheManagerImpl>(new RemoteCacheManagerImpl(properties, start_)) { } // Deprecated

RemoteCacheManager::RemoteCacheManager(const Configuration& configuration, bool start_)
  : Handle<RemoteCacheManagerImpl>(new RemoteCacheManagerImpl(configuration, start_)) { }

void RemoteCacheManager::initCache(
    RemoteCacheBase& cache, bool forceReturnValue)
{
    cache.impl = impl->createRemoteCache(forceReturnValue);
    if(!impl)
    	throw RemoteCacheNotExistException("cache doesn't exist");
}

void RemoteCacheManager::initCache(
    RemoteCacheBase& cache, const std::string& name, bool forceReturnValue)
{
    cache.impl = impl->createRemoteCache(name, forceReturnValue);
    if(!impl)
    	throw RemoteCacheNotExistException("cache doesn't exist");
}

void RemoteCacheManager::start() {
    impl->start();
}

void RemoteCacheManager::stop() {
    impl->stop();
}

bool RemoteCacheManager::isStarted() {
    return impl->isStarted();
}

const Configuration& RemoteCacheManager::getConfiguration() {
    return impl->getConfiguration();
}

}} // namespace infinispan::hotrod
