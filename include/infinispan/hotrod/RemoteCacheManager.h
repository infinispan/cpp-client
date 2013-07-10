#ifndef ISPN_HOTROD_REMOTECACHEMANAGER_H
#define ISPN_HOTROD_REMOTECACHEMANAGER_H

#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/Handle.h"
#include "infinispan/hotrod/RemoteCache.h"

//TODO: make the marshaller configurable
#include "hotrod/sys/BasicMarshaller.h"

namespace infinispan {
namespace hotrod {

class RemoteCacheManagerImpl;

class HR_EXTERN RemoteCacheManager
    : public infinispan::hotrod::Handle<RemoteCacheManagerImpl>
{
  public:
    RemoteCacheManager();

    template <class K, class V> RemoteCache<K, V> getCache() {
        RemoteCache<K, V> rcache;
        initCache(rcache);
        rcache.keyMarshaller.reset(new sys::BasicMarshaller<K>());
        rcache.valueMarshaller.reset(new sys::BasicMarshaller<V>());
        return rcache;
    }

    template <class K, class V> RemoteCache<K, V> 
	getCache(HR_SHARED_PTR<Marshaller<K> > km, HR_SHARED_PTR<Marshaller<V> > vm) {
        RemoteCache<K, V> rcache;
        initCache(rcache);
        rcache.keyMarshaller = km;
        rcache.valueMarshaller = vm;
        return rcache;
    }

  private:
    void initCache (RemoteCacheBase &cache);
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHEMANAGER_H */
