#ifndef ISPN_HOTROD_REMOTECACHEMANAGER_H
#define ISPN_HOTROD_REMOTECACHEMANAGER_H

#include "infinispan/hotrod/BasicMarshaller.h"
#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/Configuration.h"
#include "infinispan/hotrod/RemoteCounterManager.h"
#include "infinispan/hotrod/TransactionManager.h"



#include <string>
#include <map>
#include <memory>

namespace infinispan {
namespace hotrod {

class RemoteCacheManagerImpl;
class RemoteCacheManagerAdmin;

/**
 * Factory for RemoteCache objects.
 *
 * <p> <b>Lifecycle:</b> </p> In order to be able to use an instance of RemoteCache,
 * the %RemoteCacheManager must be started first.  Beside other things, this instantiates
 * connections to Hot Rod server(s). Starting the %RemoteCacheManager can be done either at
 * creation by passing start==true to constructor or after construction by calling start()
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
     *\param start_ optional boolean parameter indicating whether to start this RemoteCacheManager
     */
    explicit RemoteCacheManager(bool start_ = true);

    explicit RemoteCacheManager(
            const std::map<std::string, std::string>& configuration,
            bool start_ = true) : transactionManager(TransactionManagerLookup::lookup()) {
        init(configuration, start_);
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
     * Returns a RemoteCache instance connected to the default cache
     *
     * \param forceReturnValue if true, force all the HotRod operation that have optional return to always return a value
     * \return a RemoteCache instance connected to the default cache
     *
     */
    template <class K, class V> RemoteCache<K, V> &getCache(
            bool forceReturnValue) {
        const std::string key = forceReturnValue ? "/true" : "/false";
        if (remoteCacheMap.find(key)==remoteCacheMap.end())
        {
			auto rcache = new RemoteCache<K, V>(transactionManager,
					transactionTable, forceReturnValue,
					getConfiguration().isTransactional());
			try {
            initCache(*rcache, forceReturnValue, getConfiguration().getNearCacheConfiguration());
			} catch (...) {
				delete rcache;
				throw;
			}
			remoteCacheMap[key] = std::unique_ptr<RemoteCacheBase>(rcache);
        }
         auto cache = (RemoteCache<K, V> *)remoteCacheMap[key].get();
         cache->keyMarshaller.reset(new BasicMarshaller<K>());
         cache->valueMarshaller.reset(new BasicMarshaller<V>());
         return *cache;
    }

    /**
     * Returns a RemoteCache instance connected to the default cache
     *
     * The Configuration.forceReturnValue policy will be used
     *
     * \return a RemoteCache instance connected to the default cache
     *
     */
    template <class K, class V> RemoteCache<K, V> &getCache() {
        return getCache<K,V>(getConfiguration().isForceReturnValue());
    }

    /**
     * Returns a RemoteCache instance connected to the cache with the given name
     *
     * \param name the cache name
     * \param forceReturnValue if true, force all the HotRod operation that have optional return to always return a value
     * \return a RemoteCache instance connected to the cache with the given name
     */
	template<class K, class V> RemoteCache<K, V>& getCache(
			const std::string &name, bool forceReturnValue) {
		const std::string key =
				forceReturnValue ? name + "/true" : name + "/false";
		if (remoteCacheMap.find(key) == remoteCacheMap.end()) {
			auto rcache = new RemoteCache<K, V>(transactionManager,
					transactionTable, forceReturnValue,
					getConfiguration().isTransactional());
			try {
				initCache(*rcache, name.c_str(), forceReturnValue);
			} catch (...) {
				delete rcache;
				throw;
			}
			remoteCacheMap[key] = std::unique_ptr<RemoteCacheBase >(rcache);
		}
        auto cache = (RemoteCache<K, V> *)remoteCacheMap[key].get();
        cache->keyMarshaller.reset(new BasicMarshaller<K>());
        cache->valueMarshaller.reset(new BasicMarshaller<V>());
		return *cache;
	}

    /**
     * Returns a RemoteCache instance connected to the cache with the given name
     *
     * The Configuration.forceReturnValue policy will be used
     *
     * \param name the cache name
     * \return a RemoteCache instance connected to the cache with the given name
     */
    template <class K, class V> RemoteCache<K, V> &getCache(
            const std::string& name) {
        return getCache<K,V>(name, getConfiguration().isForceReturnValue());
    }

    /**
     * Returns a RemoteCache instance connected to the default cache
     *
     * \param km the key marshaller
     * \param kd function used as key marshaller destructor after this is no longer needed
     * \param vm the value marshaller
     * \param vd function used as value marshaller destructor after this is no longer needed
     * \param forceReturnValue if true, force all the HotRod operation that have optional return to always return a value
     * \return a RemoteCache instance connected to the default cache
     */
	template<class K, class V> RemoteCache<K, V>& getCache(Marshaller<K> *km,
			void (*kd)(Marshaller<K>*), Marshaller<V> *vm,
			void (*vd)(Marshaller<V>*), bool forceReturnValue) {
		const std::string key = forceReturnValue ? "/true" : "/false";
		if (remoteCacheMap.find(key) == remoteCacheMap.end()) {
			auto rcache = new RemoteCache<K, V>(transactionManager,
					transactionTable, forceReturnValue,
					getConfiguration().isTransactional());
			try {
				initCache(*rcache, forceReturnValue,
						getConfiguration().getNearCacheConfiguration());
			} catch (...) {
				delete rcache;
				throw;
			}
			remoteCacheMap[key] = std::unique_ptr<RemoteCacheBase>(rcache);
		}
		auto cache = (RemoteCache<K, V>*) remoteCacheMap[key].get();
		cache->keyMarshaller.reset(km);
		cache->valueMarshaller.reset(vm);
		return *cache;
	}

    /**
     * Returns a RemoteCache instance connected to the default cache
     *
     * The Configuration.forceReturnValue policy will be used
     *
     * \param km the key marshaller
     * \param kd function used as key marshaller destructor after this is no longer needed
     * \param vm the value marshaller
     * \param vd function used as value marshaller destructor after this is no longer needed
     * will indeed return a value.
     * \return a RemoteCache instance connected to the default cache
     *
     */
    template<class K, class V> RemoteCache<K, V> &getCache(
            Marshaller<K> *km, void (*kd)(Marshaller<K> *),
            Marshaller<V> *vm, void (*vd)(Marshaller<V> *)) {
        return getCache<K,V>(km, kd, vm, vd, getConfiguration().isForceReturnValue());
    }
    /**
     * Returns a RemoteCache instance connected to the cache with the given name
     *
     * \param km the key marshaller
     * \param kd function used as key marshaller destructor after this is no longer needed
     * \param vm the value marshaller
     * \param vd function used as value marshaller destructor after this is no longer needed
     * \param name the cache name to connect to on a remote Infinispan server
     * \param forceReturnValue if true, force all the HotRod operation that have optional return to always return a value
     * \return a RemoteCache instance connected to the cache with the given name
	 */
	template<class K, class V> RemoteCache<K, V>& getCache(Marshaller<K> *km,
			void (*kd)(Marshaller<K>*), Marshaller<V> *vm,
			void (*vd)(Marshaller<V>*), const std::string &name,
			bool forceReturnValue) {
		const std::string key =
				forceReturnValue ? name + "/true" : name + "/false";
		if (remoteCacheMap.find(key) == remoteCacheMap.end()) {
			auto rcache = new RemoteCache<K, V>(transactionManager,
					transactionTable, forceReturnValue,
					getConfiguration().isTransactional());
			try {
			initCache(*rcache, name.c_str(), forceReturnValue,
					getConfiguration().getNearCacheConfiguration());
			} catch (...) {
				delete rcache;
				throw;
			}
			remoteCacheMap[key] = std::unique_ptr<RemoteCacheBase >(rcache);
		}
		auto cache = (RemoteCache<K, V>*) remoteCacheMap[key].get();
		cache->keyMarshaller.reset(km);
		cache->valueMarshaller.reset(vm);
		return *cache;
	}

    /**
     * Returns a RemoteCache instance connected to the cache with the given name
     *
     * \param km the key marshaller
     * \param kd function used as key marshaller destructor after this is no longer needed
     * \param vm the value marshaller
     * \param vd function used as value marshaller destructor after this is no longer needed
     * \param name the cache name to connect to on a remote Infinispan server
     * \return a RemoteCache instance connected to the cache with the given name
     */
    template <class K, class V> RemoteCache<K, V> &getCache(
            Marshaller<K> *km, void (*kd)(Marshaller<K> *),
            Marshaller<V> *vm, void (*vd)(Marshaller<V> *),
            const std::string& name) {
        return getCache(km, kd, vm, vd, name, getConfiguration().isForceReturnValue());
    }
    /**
     * Switch the client on the main cluster
     *
     * \return ClusterStatus::SWITCHED or ClusterStatus::NOT_SWITCHED if no fully working
     * cluster is available
     */
    bool switchToDefaultCluster();

    /**
     * Switch the client on the main cluster
     *
     * \return ClusterStatus::SWITCHED or ClusterStatus::NOT_SWITCHED if the named cluster
     * doesn't exists
     */
    bool switchToCluster(std::string clusterName);

    RemoteCounterManager& getCounterManager();
    /**
     * Get the administrative interface
     *
     * \return a new instance of cache administrator
     */

    std::shared_ptr<RemoteCacheManagerAdmin> administration() {
       return newRemoteCacheManagerAdmin();
    }

    std::set<std::string> getCacheNames();
    TransactionManager& getTransactionManager() const
    {
        return transactionManager;
    }

private:
    void *impl;
    std::map<std::string, std::unique_ptr<RemoteCacheBase> > remoteCacheMap;
    std::function<void(std::string&)> cacheRemover;
    TransactionManager& transactionManager;
    TransactionTable transactionTable;

    void init(const std::map<std::string, std::string>& configuration, bool start);

    void initCache(RemoteCacheBase& cache, bool forceReturnValue, NearCacheConfiguration nc = NearCacheConfiguration());
    void initCache(RemoteCacheBase& cache, const char *name, bool forceReturnValue, NearCacheConfiguration nc = NearCacheConfiguration());

    // not implemented
    RemoteCacheManager(const RemoteCacheManager&);
    RemoteCacheManager operator=(const RemoteCacheManager&);

    std::shared_ptr<RemoteCacheManagerAdmin> newRemoteCacheManagerAdmin();

    template<typename T> static void genericDelete(T *t) { delete t; }
    template<typename T> static void genericNoDelete(T *) { }
};

}} // namespace infinispan::hotrod

#endif  /* ISPN_HOTROD_REMOTECACHEMANAGER_H */
