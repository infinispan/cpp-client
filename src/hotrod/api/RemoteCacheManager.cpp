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

RemoteCacheManager::RemoteCacheManager(bool start_)
  : impl(new RemoteCacheManagerImpl(start_)) { }

RemoteCacheManager::RemoteCacheManager(const Configuration& configuration, bool start_)
  : impl(new RemoteCacheManagerImpl(configuration, start_)) { }

void RemoteCacheManager::init(const portable::map<portable::string, portable::string> &properties, bool start_) {
    impl = new RemoteCacheManagerImpl(
            properties.std_map<std::string, portable::string::convert, std::string, portable::string::convert>(
                    portable::string::convert(), portable::string::convert()), start_);
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

}} // namespace infinispan::hotrod
