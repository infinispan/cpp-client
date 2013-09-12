#ifndef ISPN_HOTROD_REMOTECACHEIMPL_H
#define ISPN_HOTROD_REMOTECACHEIMPL_H



#include "infinispan/hotrod/types.h"
#include "infinispan/hotrod/ScopedBuffer.h"
#include "infinispan/hotrod/RemoteCacheBase.h"
#include "hotrod/impl/MetadataValueImpl.h"

namespace infinispan {
namespace hotrod {

namespace operations {
class OperationsFactory;
}

class RemoteCacheImpl
{
  public:
    RemoteCacheImpl(RemoteCacheBase& base, const std::string& name);
    void get(const void* key, void *buf);
    void put(const void *key, const void* val, uint64_t life, uint64_t idle, void* b);
    void putIfAbsent(const void *key, const void* val, uint64_t life, uint64_t idle, void* b);
    void replace(const void *key, const void* val, uint64_t life, uint64_t idle, void* b);
    void remove(const void* key, void* buf);
    void containsKey(const void* key, bool* res);
    void replaceWithVersion(const void* k, const void* v, uint64_t version, uint64_t life, uint64_t idle, bool* res);
    void removeWithVersion(const void* k, uint64_t version, bool* res);
    void getWithMetadata(const void *key, void* vbuf, MetadataValue* metadata);
    void getBulk(int size, std::map<void*, void*>* mbuf);
    void keySet(int scope, std::set<void*>* result);
    void stats(std::map<std::string,std::string>* stats);
    void clear();

    void ping();

    void init(const std::string& name, operations::OperationsFactory* operationsFactory);
    void init(const RemoteCacheImpl &other);

    void withFlags(Flag flag);

    const std::string& getName() const;

  private:
    RemoteCacheBase& remoteCacheBase;

    HR_SHARED_PTR<operations::OperationsFactory> operationsFactory;
    std::string name;

    void applyDefaultExpirationFlags(uint64_t lifespan, uint64_t maxIdle);

};

}} // namespace infinispan::hotrod

#endif  /* ISPN_HOTROD_REMOTECACHEIMPL_H */

