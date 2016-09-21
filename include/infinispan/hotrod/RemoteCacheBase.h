#ifndef ISPN_HOTROD_REMOTECACHEBASE_H
#define ISPN_HOTROD_REMOTECACHEBASE_H



#include <infinispan/hotrod/CacheTopologyInfo.h>
#include <infinispan/hotrod/ClientListener.h>
#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/Flag.h"
#include "infinispan/hotrod/MetadataValue.h"
#include "infinispan/hotrod/portable.h"
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable:4267 4244)
#endif
#include "query.pb.h"
#if _MSC_VER
#pragma warning(pop)
#endif
#include "infinispan/hotrod/ClientListener.h"

#include <map>
#include <set>
#include <vector>

using namespace org::infinispan::query::remote::client;
using namespace infinispan::hotrod::event;

namespace infinispan {
namespace hotrod {

namespace operations {
class OperationsFactory;
}

namespace event {
template <class K, class V> class CacheClientListener;
}

typedef void (*MarshallHelperFn) (void*, const void*, std::vector<char> &);
typedef void* (*UnmarshallHelperFn) (void*, const std::vector<char> &);

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
    HR_EXTERN uint64_t  base_size();
    HR_EXTERN void  base_withFlags(Flag flag);
    HR_EXTERN std::vector<unsigned char> base_execute(const std::string &cmdName,  const std::map<std::string,std::string>& args);
    HR_EXTERN CacheTopologyInfo base_getCacheTopologyInfo();
    HR_EXTERN QueryResponse base_query(const QueryRequest &qr);
    HR_EXTERN std::vector<unsigned char> base_query_char(std::vector<unsigned char> qr, size_t size);
	HR_EXTERN void base_addClientListener(ClientListener &clientListener, const std::vector<std::vector<char> > filterFactoryParam, const std::vector<std::vector<char> > converterFactoryParams, const std::function<void()> &recoveryCallback);

    RemoteCacheBase() {}
    HR_EXTERN void setMarshallers(void* rc, MarshallHelperFn kf, MarshallHelperFn vf, UnmarshallHelperFn ukf, UnmarshallHelperFn uvf);

private:
    portable::counting_ptr<portable::counted_object> impl; // pointer to RemoteCacheImpl;
    void *remoteCachePtr; // TODO: pointer to self, is it really necessary?
    MarshallHelperFn baseKeyMarshallFn;
    MarshallHelperFn baseValueMarshallFn;
    HR_EXTERN void baseKeyMarshall(const void* k, std::vector<char> &buf);
    HR_EXTERN void baseValueMarshall(const void* v, std::vector<char> &buf);

    UnmarshallHelperFn baseKeyUnmarshallFn;
    UnmarshallHelperFn baseValueUnmarshallFn;
    HR_EXTERN void* baseKeyUnmarshall(const std::vector<char> &buf);
    HR_EXTERN void* baseValueUnmarshall(const std::vector<char> &buf);

friend class RemoteCacheManager;
friend class RemoteCacheImpl;
friend class KeyUnmarshallerFtor;
friend class ValueUnmarshallerFtor;
template <class K, class V>
friend class ::infinispan::hotrod::event::CacheClientListener;
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHEBASE_H */
