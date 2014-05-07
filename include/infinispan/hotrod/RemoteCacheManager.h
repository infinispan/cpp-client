#ifndef ISPN_HOTROD_REMOTECACHEMANAGER_H
#define ISPN_HOTROD_REMOTECACHEMANAGER_H

#include "infinispan/hotrod/BasicMarshaller.h"
#include "infinispan/hotrod/portable.h"
#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/Configuration.h"

#include <string>
#include <map>

namespace infinispan {
namespace hotrod {

class RemoteCacheManagerImpl;

/**
 * Factory for RemoteCache objects.
 *
 * <p> <b>Lifecycle:</b> </p> In order to be able to use an instance of RemoteCache,
 * the %RemoteCacheManager must be started first.  Beside other things, this instantiates
 * connections to Hot Rod server(s). Starting the %RemoteCacheManager can be done either at
 * creation by passing start==true to constructor or after construction by calling ::start
 * method.</p>
 *
 * <p>%RemoteCacheManager is an "expensive" object, as it manages a set of persistent TCP
 * connections to the Hot Rod servers. It is recommended to only have one instance of it
 * per runtime and to cache it between calls to the server (i.e. remoteCache %operations).</p>
 *
 * <p>After you are done with RemoteCacheManager stop needs to be called explicitly in order
 * to release all the resources (e.g. threads, TCP connections).</p>

 * <p><b>%Configuration:</b></p>
 * The cache manager is configured through a Configuration object passed to the constructor.
 *
 */
class HR_EXTERN RemoteCacheManager
{
public:

    /**
     * Creates a new instance of RemoteCacheManager using the default %Configuration
     * object an an optional boolean parameter indicating whether to also start this
     * RemoteCacheManager as well.
     *
     *\param start optional boolean parameter indicating whether to start this RemoteCacheManager
     */
    explicit RemoteCacheManager(bool start_ = true);

    explicit RemoteCacheManager(
            const std::map<std::string, std::string>& configuration,
            bool start_ = true) {
        init(portable::map<portable::string, portable::string>(
                configuration, portable::string::convert(), portable::string::convert()), start_);
    }

    /**
     * Creates a new instance of RemoteCacheManager given an accompanying
     * %Configuration object an an optional boolean parameter indicating whether to
     * also start this RemoteCacheManager as well.
     *
     *\param configuration the Configuration reference to use for configuration of this
     *RemoteCacheManager
     *\param start optional boolean parameter indicating whether to start this RemoteCacheManager
     */
    explicit RemoteCacheManager(
        const Configuration& configuration,
        bool start = true);

    ~RemoteCacheManager();

    /**
     * Starts this RemoteCacheManager if it already has not been started
     *
     */
    void start();

    /**
     * Stops this RemoteCacheManager and releases all related resource
     *
     */
    void stop();

    /**
     * Returns true if this RemoteCacheManager has already been started
     *
     *
     * \return true if this RemoteCacheManager has already been started, false otherwise
     *
     */
    bool isStarted();

    /**
     * Returns the Configuration instance used for configuration of this RemoteCacheManager
     *
     *
     * \return the Configuration instance of this RemoteCacheManager
     *
     */
    const Configuration& getConfiguration();

    /**
     * Returns the default RemoteCache given an optional forceReturnValue boolean parameter.
     * The optional forceReturnValue parameter is false by default meaning that each cache
     * operation that optionally returns a value will return a NULL. In such case you could
     * still force operation to return a value on per invocation basis using the appropriate
     * Flag. However, if you want to force return value on all cache %operations
     * that optionally return a value set forceReturnValue parameter to true.
     *
     *
     * \param forceReturnValue if true all cache %operations that optionally return a value
     * will indeed return a value.
     * \return the default RemoteCache to interact with on a remote Infinispan server
     *
     */
    template <class K, class V> RemoteCache<K, V> getCache(
        bool forceReturnValue = false)
    {
        RemoteCache<K, V> rcache;
        initCache(rcache, forceReturnValue);
        rcache.keyMarshaller.reset(new BasicMarshaller<K>(), &Marshaller<K>::destroy);
        rcache.valueMarshaller.reset(new BasicMarshaller<V>(), &Marshaller<V>::destroy);
        return rcache;
    }

    /**
     * Returns RemoteCache for the given cache name and an optional forceReturnValue boolean
     * parameter. The optional forceReturnValue parameter is false by default meaning that
     * each cache operation that optionally returns a value will return a NULL. In such case
     * you could still force operation to return a value on per invocation basis using the
     * appropriate Flag. However, if you want to force return value on all cache %operations
     * that optionally return a value set forceReturnValue parameter to true.
     *
     *
     * \param name the cache name to connect to on a remote Infinispan server
     * \param forceReturnValue if true all cache %operations that optionally return a value
     * will indeed return a value.
     * \return the RemoteCache to interact with on a remote Infinispan server
     *
     */
    template <class K, class V> RemoteCache<K, V> getCache(
        const std::string& name, bool forceReturnValue = false)
    {
        RemoteCache<K, V> rcache;
        initCache(rcache, name.c_str(), forceReturnValue);
        rcache.keyMarshaller.reset(new BasicMarshaller<K>(), &Marshaller<K>::destroy);
        rcache.valueMarshaller.reset(new BasicMarshaller<V>(), &Marshaller<V>::destroy);
        return rcache;
    }

    /**
     * Returns the default RemoteCache given the key and the value marshallers, the cache name
     * and an optional forceReturnValue boolean parameter.
     *
     * The optional forceReturnValue parameter is false by default meaning that each cache
     * operation that optionally returns a value will return a NULL. In such case you could
     * still force operation to return a value on per invocation basis using the appropriate
     * Flag. However, if you want to force return value on all cache %operations that optionally
     * return a value set forceReturnValue parameter to true.
     *
     * \param km the key marshaller
     * \param km function used as key marshaller destructor after this is no longer needed
     * \param vm the value marshaller
     * \param vd function used as value marshaller destructor after this is no longer needed
     * \param forceReturnValue if true all cache %operations that optionally return a value
     * will indeed return a value.
     * \return the default RemoteCache to interact with on a remote Infinispan server
     *
     */
    template <class K, class V> RemoteCache<K, V> getCache(
        Marshaller<K> *km, void (*kd)(Marshaller<K> *),
        Marshaller<V> *vm, void (*vd)(Marshaller<V> *),
        bool forceReturnValue = false)
    {
        RemoteCache<K, V> rcache;
        initCache(rcache, forceReturnValue);
        rcache.keyMarshaller.reset(km, kd);
        rcache.valueMarshaller.reset(vm, vd);
        return rcache;
    }

    /**
     * Returns RemoteCache for the given key and value marshallers, the cache name and an
     * optional forceReturnValue boolean parameter.
     *
     * The optional forceReturnValue parameter is false by default meaning that each cache
     * operation that optionally returns a value will return a NULL. In such case you could
     * still force operation to return a value on per invocation basis using the appropriate
     * Flag. However, if you want to force return value on all cache %operations
     * that optionally return a value set forceReturnValue parameter to true.
     *
     * \param km the key marshaller
     * \param km function used as key marshaller destructor after this is no longer needed
     * \param vm the value marshaller
     * \param vd function used as value marshaller destructor after this is no longer needed
     * \param name the cache name to connect to on a remote Infinispan server
     * \param forceReturnValue if true all cache %operations that optionally return a value
     *  will indeed return a value.
     * \return the RemoteCache to interact with on a remote Infinispan server
     *
     */
    template <class K, class V> RemoteCache<K, V> getCache(
        Marshaller<K> *km, void (*kd)(Marshaller<K> *),
        Marshaller<V> *vm, void (*vd)(Marshaller<V> *),
        const std::string& name, bool forceReturnValue = false)
    {
        RemoteCache<K, V> rcache;
        initCache(rcache, name.c_str(), forceReturnValue);
        rcache.keyMarshaller.reset(km, kd);
        rcache.valueMarshaller.reset(vm, vd);
        return rcache;
    }

    // DEPRECATED
    template <class K, class V> RemoteCache<K, V> getCache(
        HR_SHARED_PTR<Marshaller<K> > km, HR_SHARED_PTR<Marshaller<V> > vm,
        bool forceReturnValue = false)
    {
        RemoteCache<K, V> rcache;
        initCache(rcache, forceReturnValue);
        rcache.keyMarshallerPtr.reset(new portable::counted_wrapper<HR_SHARED_PTR<Marshaller<K> > >(km), &genericDelete);
        rcache.valueMarshallerPtr.reset(new portable::counted_wrapper<HR_SHARED_PTR<Marshaller<K> > >(vm), &genericDelete);
        rcache.keyMarshaller.reset(&*km, &genericNoDelete);
        rcache.valueMarshaller.reset(&*vm, &genericNoDelete);
        return rcache;
    }

    template <class K, class V> RemoteCache<K, V> getCache(
        HR_SHARED_PTR<Marshaller<K> > km, HR_SHARED_PTR<Marshaller<V> > vm,
        const std::string& name, bool forceReturnValue = false)
    {
        RemoteCache<K, V> rcache;
        initCache(rcache, name.c_str(), forceReturnValue);
        rcache.keyMarshallerPtr.reset(new portable::counted_wrapper<HR_SHARED_PTR<Marshaller<K> > >(km), &genericDelete);
        rcache.valueMarshallerPtr.reset(new portable::counted_wrapper<HR_SHARED_PTR<Marshaller<K> > >(vm), &genericDelete);
        rcache.keyMarshaller.reset(&*km, &genericNoDelete);
        rcache.valueMarshaller.reset(&*vm, &genericNoDelete);
        return rcache;
    }

private:
    void *impl;

    void init(const portable::map<portable::string, portable::string>& configuration, bool start);

    void initCache(RemoteCacheBase& cache, bool forceReturnValue);
    void initCache(RemoteCacheBase& cache, const char *name, bool forceReturnValue);

    // not implemented
    RemoteCacheManager(const RemoteCacheManager&);
    RemoteCacheManager operator=(const RemoteCacheManager&);

    template<typename T> static void genericDelete(T *t) { delete t; }
    template<typename T> static void genericNoDelete(T *) { }
};

}} // namespace infinispan::hotrod

#endif  /* ISPN_HOTROD_REMOTECACHEMANAGER_H */
