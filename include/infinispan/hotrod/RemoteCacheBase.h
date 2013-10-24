#ifndef ISPN_HOTROD_REMOTECACHEBASE_H
#define ISPN_HOTROD_REMOTECACHEBASE_H



#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/Handle.h"
#include "infinispan/hotrod/Flag.h"
#include "infinispan/hotrod/MetadataValue.h"

#include <map>
#include <set>

namespace infinispan {
namespace hotrod {

namespace operations {
class OperationsFactory;
}

class RemoteCacheImpl;
class RemoteCacheManagerImpl;

typedef void (*MarshallHelperFn) (void*, const void*, void*);
typedef void* (*UnmarshallHelperFn) (void*, const void*);

class HR_EXTERN RemoteCacheBase
  : public infinispan::hotrod::Handle<RemoteCacheImpl>
{
  public:
    void base_get(const void *key, void *rbuf);
    void base_put(const void *key, const void *value, int64_t life, int64_t idle, void *buf);
    void base_putIfAbsent(const void *key, const void *value, int64_t life, int64_t idle, void *buf);
    void base_replace(const void *key, const void *value, int64_t life, int64_t idle, void *buf);
    void base_remove(const void *key, void *rbuf);
    void base_containsKey(const void *key, bool *res);
    void base_ping();
    void base_replaceWithVersion(const void *key, const void *value, int64_t version, int64_t life, int64_t idle, bool *res);
    void base_removeWithVersion(const void *key, int64_t version, bool *res);
    void base_getWithVersion(const void* key, void* vbuf, VersionedValue* version);
    void base_getWithMetadata(const void* key, void* vbuf, MetadataValue* metadata);
    void base_getBulk(int size, std::map<void*, void*>* mbuf);
    void base_keySet(int scope, std::set<void*>* sbuf);
    void base_stats(std::map<std::string,std::string>* sbuf);
    void base_clear();
    void base_withFlags(Flag flag);

    void init(operations::OperationsFactory* operationFactory);

 protected:
    RemoteCacheBase();
    void setMarshallers(void* rc, MarshallHelperFn kf, MarshallHelperFn vf, UnmarshallHelperFn ukf, UnmarshallHelperFn uvf);

  private:
    void *remoteCachePtr;
    MarshallHelperFn baseKeyMarshallFn;
    MarshallHelperFn baseValueMarshallFn;
    void baseKeyMarshall(const void* k, void *buf);
    void baseValueMarshall(const void* v, void *buf);

    UnmarshallHelperFn baseKeyUnmarshallFn;
    UnmarshallHelperFn baseValueUnmarshallFn;
    void* baseKeyUnmarshall(const void* buf);
    void* baseValueUnmarshall(const void* buf);

  friend class RemoteCacheManager;
  friend class RemoteCacheImpl;
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHEBASE_H */
