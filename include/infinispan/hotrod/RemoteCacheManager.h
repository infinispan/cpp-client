#ifndef ISPN_HOTROD_REMOTECACHEMANAGER_H
#define ISPN_HOTROD_REMOTECACHEMANAGER_H



#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/Handle.h"
#include "infinispan/hotrod/RemoteCache.h"

#include "hotrod/sys/BasicMarshaller.h"

#include <string>
#include <map>

namespace infinispan {
namespace hotrod {

class RemoteCacheManagerImpl;
class Configuration;

class HR_EXTERN RemoteCacheManager : public Handle<RemoteCacheManagerImpl>
{
  public:
    explicit RemoteCacheManager(bool start = true);
    explicit RemoteCacheManager(
        const std::map<std::string, std::string>& configuration,
        bool start = true);

    void start();
    void stop();
    bool isStarted();
    // TODO: change to std::map?
    const Configuration& getConfiguration();

    template <class K, class V> RemoteCache<K, V> getCache(
        bool forceReturnValue = false)
    {
        RemoteCache<K, V> rcache("");
        initCache(rcache, forceReturnValue);
        rcache.keyMarshaller.reset(new sys::BasicMarshaller<K>());
        rcache.valueMarshaller.reset(new sys::BasicMarshaller<V>());
        return rcache;
     }

    template <class K, class V> RemoteCache<K, V> getCache(
        const std::string& name, bool forceReturnValue = false)
    {
        RemoteCache<K, V> rcache(name);
        initCache(rcache, forceReturnValue);
        rcache.keyMarshaller.reset(new sys::BasicMarshaller<K>());
        rcache.valueMarshaller.reset(new sys::BasicMarshaller<V>());
        return rcache;
    }

    template <class K, class V> RemoteCache<K, V> getCache(
        HR_SHARED_PTR<Marshaller<K> > km, HR_SHARED_PTR<Marshaller<V> > vm,
        bool forceReturnValue = false)
    {
        RemoteCache<K, V> rcache("");
        initCache(rcache, forceReturnValue);
        rcache.keyMarshaller = km;
        rcache.valueMarshaller = vm;
        return rcache;    }

    template <class K, class V> RemoteCache<K, V> getCache(
        HR_SHARED_PTR<Marshaller<K> > km, HR_SHARED_PTR<Marshaller<V> > vm,
        const std::string& name, bool forceReturnValue = false)
    {
        RemoteCache<K, V> rcache(name);
        initCache(rcache, forceReturnValue);
        rcache.keyMarshaller = km;
        rcache.valueMarshaller = vm;
        return rcache;
    }

  private:
    void initCache(RemoteCacheBase& cache, bool forceReturnValue);

    // not implemented
    RemoteCacheManager(const RemoteCacheManager&);
    RemoteCacheManager operator=(const RemoteCacheManager&);
};

}} // namespace infinispan::hotrod

#endif  /* ISPN_HOTROD_REMOTECACHEMANAGER_H */
