#ifndef ISPN_HOTROD_REMOTECACHEIMPL_H
#define ISPN_HOTROD_REMOTECACHEIMPL_H



#include "infinispan/hotrod/types.h"
#include "infinispan/hotrod/ScopedBuffer.h"
#include "infinispan/hotrod/RemoteCacheBase.h"
#include "hotrod/impl/MetadataValueImpl.h"
#include "hotrod/impl/operations/PingOperation.h"

namespace infinispan {
namespace hotrod {

namespace operations {
class OperationsFactory;
}

class RemoteCacheManagerImpl;

class RemoteCacheImpl
{
  public:
    RemoteCacheImpl(RemoteCacheManagerImpl& rcm, const std::string& name);
    void get(RemoteCacheBase& rcb, const void* key, void *buf);
    void put(RemoteCacheBase& rcb, const void *key, const void* val, uint64_t life, uint64_t idle, void* b);
    void putIfAbsent(RemoteCacheBase& rcb, const void *key, const void* val, uint64_t life, uint64_t idle, void* b);
    void replace(RemoteCacheBase& rcb, const void *key, const void* val, uint64_t life, uint64_t idle, void* b);
    void remove(RemoteCacheBase& rcb, const void* key, void* buf);
    void containsKey(RemoteCacheBase& rcb, const void* key, bool* res);
    void replaceWithVersion(RemoteCacheBase& rcb, const void* k, const void* v, uint64_t version, uint64_t life, uint64_t idle, bool* res);
    void removeWithVersion(RemoteCacheBase& rcb, const void* k, uint64_t version, bool* res);
    void getWithMetadata(RemoteCacheBase& rcb, const void *key, void* vbuf, MetadataValue* metadata);
    void getBulk(RemoteCacheBase& rcb, int size, std::map<void*, void*>* mbuf);
    void keySet(RemoteCacheBase& rcb, int scope, std::set<void*>* result);
    void stats(std::map<std::string,std::string>* stats);
    void clear();

    operations::PingResult ping();

    void init(operations::OperationsFactory* operationsFactory);

    void withFlags(Flag flag);

    const std::string& getName() const;

  private:
    RemoteCacheManagerImpl& remoteCacheManager;

    HR_SHARED_PTR<operations::OperationsFactory> operationsFactory;
    std::string name;

    void applyDefaultExpirationFlags(uint64_t lifespan, uint64_t maxIdle);
    void assertRemoteCacheManagerIsStarted();
};

}} // namespace infinispan::hotrod

#endif  /* ISPN_HOTROD_REMOTECACHEIMPL_H */

