#ifndef ISPN_HOTROD_REMOTECACHEBASE_H
#define ISPN_HOTROD_REMOTECACHEBASE_H



#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/Flag.h"
#include "infinispan/hotrod/MetadataValue.h"
#include "infinispan/hotrod/ScopedBuffer.h"

#include <map>
#include <set>

namespace infinispan {
namespace hotrod {

namespace operations {
class OperationsFactory;
}

typedef void (*MarshallHelperFn) (void*, const void*, ScopedBuffer &);
typedef void* (*UnmarshallHelperFn) (void*, const ScopedBuffer &);

class KeyUnmarshallerFtor;
class ValueUnmarshallerFtor;

class RemoteCacheBase
{
protected:
    HR_EXTERN const char *base_getName();
    HR_EXTERN void *base_get(const void *key);
    HR_EXTERN void *base_put(const void *key, const void *value, int64_t life, int64_t idle);
    HR_EXTERN void *base_putIfAbsent(const void *key, const void *value, int64_t life, int64_t idle);
    HR_EXTERN void *base_replace(const void *key, const void *value, int64_t life, int64_t idle);
    HR_EXTERN void *base_remove(const void *key);
    HR_EXTERN bool  base_containsKey(const void *key);
    HR_EXTERN void  base_ping();
    HR_EXTERN bool  base_replaceWithVersion(const void *key, const void *value, int64_t version, int64_t life, int64_t idle);
    HR_EXTERN bool  base_removeWithVersion(const void *key, int64_t version);
    HR_EXTERN void *base_getWithVersion(const void* key, VersionedValue* version);
    HR_EXTERN void *base_getWithMetadata(const void* key, MetadataValue* metadata);
    HR_EXTERN void  base_getBulk(int size, portable::map<void*, void*> &mbuf);
    HR_EXTERN void  base_keySet(int scope, portable::vector<void*> &sbuf);
    HR_EXTERN void  base_stats(portable::map<portable::string,portable::string> &sbuf);
    HR_EXTERN void  base_clear();
    HR_EXTERN void  base_withFlags(Flag flag);

    RemoteCacheBase() {}
    HR_EXTERN void setMarshallers(void* rc, MarshallHelperFn kf, MarshallHelperFn vf, UnmarshallHelperFn ukf, UnmarshallHelperFn uvf);

private:
    portable::counting_ptr<portable::counted_object> impl; // pointer to RemoteCacheImpl;
    void *remoteCachePtr; // TODO: pointer to self, is it really necessary?
    MarshallHelperFn baseKeyMarshallFn;
    MarshallHelperFn baseValueMarshallFn;
    HR_EXTERN void baseKeyMarshall(const void* k, ScopedBuffer &buf);
    HR_EXTERN void baseValueMarshall(const void* v, ScopedBuffer &buf);

    UnmarshallHelperFn baseKeyUnmarshallFn;
    UnmarshallHelperFn baseValueUnmarshallFn;
    HR_EXTERN void* baseKeyUnmarshall(const ScopedBuffer &buf);
    HR_EXTERN void* baseValueUnmarshall(const ScopedBuffer &buf);

friend class RemoteCacheManager;
friend class RemoteCacheImpl;
friend class KeyUnmarshallerFtor;
friend class ValueUnmarshallerFtor;
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHEBASE_H */
