#include "infinispan/hotrod/RemoteCacheManager.h"
#include "hotrod/impl/RemoteCacheManagerImpl.h"
#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/operations/OperationsFactory.h"

namespace infinispan {
namespace hotrod {


using namespace protocol;
using namespace transport;
using namespace operations;

#define IMPL ((RemoteCacheManagerImpl *) impl)

void removeFromMap(std::map<std::string, std::unique_ptr<RemoteCacheBase> >& map, std::string& name) {
    map.erase(name+"/false");
    map.erase(name+"/true");
}

RemoteCacheManager::RemoteCacheManager(bool start_)
  : impl(new RemoteCacheManagerImpl(start_)) {
    cacheRemover = std::bind(removeFromMap, std::ref(remoteCacheMap), std::placeholders::_1);
}

RemoteCacheManager::RemoteCacheManager(const Configuration& configuration, bool start_)
  : impl(new RemoteCacheManagerImpl(configuration, start_)) {
    cacheRemover = std::bind(removeFromMap, std::ref(remoteCacheMap), std::placeholders::_1);
}

void RemoteCacheManager::init(const std::map<std::string, std::string> &properties, bool start_) {
    cacheRemover = std::bind(removeFromMap, std::ref(remoteCacheMap), std::placeholders::_1);
    impl = new RemoteCacheManagerImpl(properties, start_);
}

RemoteCounterManager& RemoteCacheManager::getCounterManager() {
    return IMPL->getRemoteCounterManager();
}

RemoteCacheManager::~RemoteCacheManager() {
	if (IMPL)
		delete IMPL;
	impl=nullptr;
}

void RemoteCacheManager::initCache(
    RemoteCacheBase& cache, bool forceReturnValue, NearCacheConfiguration nc)
{
    cache.impl = IMPL->createRemoteCache(forceReturnValue, nc);
}

void RemoteCacheManager::initCache(
    RemoteCacheBase& cache, const char *name, bool forceReturnValue, NearCacheConfiguration nc)
{
    cache.impl = IMPL->createRemoteCache(std::string(name), forceReturnValue, nc);
}

void RemoteCacheManager::start() {
    IMPL->start();
}

void RemoteCacheManager::stop() {
    IMPL->stop();
}

bool RemoteCacheManager::isStarted() {
    return IMPL->isStarted();
}

const Configuration& RemoteCacheManager::getConfiguration() {
    return IMPL->getConfiguration();
}

bool RemoteCacheManager::switchToDefaultCluster()
{
  return IMPL->clusterSwitch(Configuration::DEFAULT_CLUSTER_NAME);
}
bool RemoteCacheManager::switchToCluster(std::string clusterName)
{
	return IMPL->clusterSwitch(clusterName);
}

std::shared_ptr<RemoteCacheManagerAdmin> RemoteCacheManager::newRemoteCacheManagerAdmin() {
    return IMPL->newRemoteCacheManagerAdmin(*this, cacheRemover);
}

std::set<std::string> RemoteCacheManager::getCacheNames() {
    return administration()->getCacheNames();
}

}} // namespace infinispan::hotrod
