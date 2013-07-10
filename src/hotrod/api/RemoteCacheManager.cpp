#include "infinispan/hotrod/RemoteCacheManager.h"
#include "hotrod/impl/RemoteCacheManagerImpl.h"

namespace infinispan {
namespace hotrod {

RemoteCacheManager::RemoteCacheManager() : 
    Handle<RemoteCacheManagerImpl>(new RemoteCacheManagerImpl()) {}
    
    void RemoteCacheManager::initCache (RemoteCacheBase &/*cache*/) {}

}} /* namespace */
