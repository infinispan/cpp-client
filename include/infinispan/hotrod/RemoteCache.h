#ifndef ISPN_HOTROD_REMOTECACHE_H
#define ISPN_HOTROD_REMOTECACHE_H

#include "infinispan/hotrod/RemoteCacheBase.h"
#include "infinispan/hotrod/Marshaller.h"
#include "infinispan/hotrod/Flag.h"
#include "infinispan/hotrod/MetadataValue.h"
#include "infinispan/hotrod/TimeUnit.h"
#include "infinispan/hotrod/VersionedValue.h"
#include "infinispan/hotrod/Version.h"
#include "infinispan/hotrod/exceptions.h"
#include "infinispan/hotrod/ClientListener.h"
#include "infinispan/hotrod/Query.h"
#include "infinispan/hotrod/QueryUtils.h"
#include "infinispan/hotrod/ContinuousQueryListener.h"
#include "infinispan/hotrod/BasicTypesProtoStreamMarshaller.h"
#include "infinispan/hotrod/JBossMarshaller.h"
#include "infinispan/hotrod/RemoteExecution.h"
#include <cmath>
#include <set>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <functional>
#include <future>
#include <iterator>

namespace infinispan
{
namespace hotrod
{

/**
 * Provides remote reference to a cache residing on a Hot Rod server/cluster.
 *
 * <p>%RemoteCache is intended to be similar to Infinispan Java %RemoteCache interface as much as possible. Just
 * like %RemoteCache interface in its Java HotRod counterpart %RemoteCache features:</p>
 *
 * <p>
 * <b>New methods</b>: Although some methods are very simila to Java Map interface, %RemoteCache also adds new
 * methods to optimize/reduce network traffic: e.g. versioned put operation.</p>
 *
 * <p><b>Concurrency</b>: implementations will support multi-threaded access.</p>
 *
 * <p><b>Return values</b>: previously existing values for certain methods are not returned, NULL
 * is returned instead unless method is using fluent variant withFlags (see below).</p>
 *
 * <p><b>Synthetic methods</b>: aggregate methods are being implemented based on other Hot Rod %operations. For example,
 * putAll method is implemented using  multiple individual puts. Therefore these methods are not atomic and that they are
 * costly, e.g. as the number of network round-trips is not one, but the size of the added map.</p>
 *
 * <p>It is possible change the default method behavior using Flag enum. For example:
 * <pre>
 *      %RemoteCache cache = ...;
 *      std::auto_ptr<std::string> rv(cache.withFlags(FORCE_RETURN_VALUE).put(k,v));
 * </pre>
 *
 * In the previous example Flag enum FORCE_RETURN_VALUE will make the client to also return previously existing value
 * associated with <tt>k</tt> key. If this flag would not be present, %RemoteCache would return (by default) <tt>NULL</tt>.
 * This is in order to avoid fetching a possibly large object from the remote server, which might not be needed.</p>
 *
 */
template<class K, class V> class RemoteCache: private RemoteCacheBase
{
private:
#ifndef SWIG      // Let SWIG ignore this method
    template<typename Function>
    inline std::future<typename std::result_of<Function()>::type> goAsync(Function&& f,
            std::function<typename std::result_of<Function()>::type(typename std::result_of<Function()>::type)> success,
            std::function<typename std::result_of<Function()>::type(std::exception&)> fail)
    {
        auto fq = [=]
        {   try
            {   return success==0 ? f() : success(f());}
            catch (std::exception& ex)
            {   if (fail!=0)
                {   return fail(ex);}
                else
                {   throw ex;}}
        };
        return std::async(fq);
    }

    template<typename Function>
    inline std::future<void> goAsync(Function&& f, std::function<void()> success,
            std::function<void(std::exception&)> fail)
    {
        auto fq = [=]
        {   try
            {   if (success==0) f();
                else success();}
            catch (std::exception& ex)
            {   if (fail!=0)
                {   fail(ex);}
                else
                {   throw ex;}}
        };
        return std::async(fq);
    }

#endif

public:

    /**
     * Retrieves the name of the cache
     *
     * \return the name of the cache
     */
    std::string getName()
    {
        return std::string(base_getName());
    }

    /**
     * Retrieves the version of Infinispan
     *
     * \return a version string
     */
    std::string getVersion()
    {
        return Version::getVersionCString();
    }

    /**
     * Retrieves the protocol version
     *
     * \return protocol version as string
     */
    std::string getProtocolVersion()
    {
        return Version::getProtocolVersionCString();
    }

    /**
     * Returns the value to which the specified key is mapped, or NULL if this cache contains no mapping for the key.
     *
     * \param key the key whose associated value is to be returned
     * \return the value to which the specified key is mapped, or NULL if this map contains no mapping for the key
     *
     */
    V* get(const K& key)
    {
        return (V *) base_get(&key);
    }

private:
    V* get_async(const K& key)
    {
        return (V *) base_get(&key);
    }

public:

    /**
     * Returns a map of key,value pairs according to the set of keys passed in input
     *
     * \param keySet the set of the keys whose associated values are to be returned
     * \return a map containing the key, value pairs
     *
     */
    std::map<std::shared_ptr<K>, std::shared_ptr<V> > getAll(const std::set<K>& keySet)
    {
        std::set<std::vector<char> > keySetMarshalled;
        for (auto item = keySet.begin(); item != keySet.end(); item++)
        {
            std::vector<char> v;
            this->keyMarshaller->marshall(*item, v);
            keySetMarshalled.insert(v);
        }
        auto marshalledResult = base_getAll(keySetMarshalled);
        std::map<std::shared_ptr<K>, std::shared_ptr<V> > result;
        for (auto item : marshalledResult)
        {
            std::shared_ptr<K> rk(this->keyMarshaller->unmarshall(item.first));
            std::shared_ptr<V> rv(this->keyMarshaller->unmarshall(item.second));
            result[rk] = rv;
        }
        return result;
    }

    /**
     * Asynchronous version of get. Executes function f when get() returns
     *
     *    See the synchronous doc for parameters not explained here
     * \param key the key whose associated value is to be returned
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future that will contain a pointer to the requested value or null
     *
     */
    std::future<V*> getAsync(const K& key, std::function<V* (V*)> success = nullptr,
            std::function<V* (std::exception&)> fail = nullptr)
    {
        const K* pKey = &key;
        auto currTx = transactionManager.getCurrentTransaction();
        std::function<V* (void)> f = [=]
        {   return (V*)this->base_get(pKey, currTx);};
        return goAsync(f, success, fail);
    }

    /**
     * <p>Associates the specified value with the specified key in this cache.</p>
     *
     * <p>If the cache previously contained a mapping for the key, the old value is replaced by the
     * specified value.</p>
     *
     * <p>If the return value of this operation will be ignored by the application,
     * the user is strongly encouraged to use the IGNORE_RETURN_VALUES Flag
     * when invoking this method in order to make it behave as efficiently
     * as possible (i.e. avoiding needless remote or network calls).</p>
     *
     * \param key  key with which the specified value is to be associated
     * \param val  value value to be associated with the specified key
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param maxIdle  the maximum amount of time this entry is allowed to be idle before it is considered as expired
     *
     * \return the previous value associated with key, or NULL if there was no mapping for key.
     *
     */
    V* put(const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
        return put(key, val, lifespan, SECONDS, maxIdle, SECONDS);
    }
    /**
     * <p>Associates the specified value with the specified key in this cache.</p>
     *
     * <p>If the cache previously contained a mapping for the key, the old value is replaced by the
     * specified value.</p>
     *
     * <p>If the return value of this operation will be ignored by the application,
     * the user is strongly encouraged to use the IGNORE_RETURN_VALUES Flag
     * when invoking this method in order to make it behave as efficiently
     * as possible (i.e. avoiding needless remote or network calls).</p>
     *
     * \param key  key with which the specified value is to be associated
     * \param val  value value to be associated with the specified key
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     *
     * \return the previous value associated with key, or NULL if there was no mapping for key.
     *
     */
    V* put(const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit)
    {
        return put(key, val, lifespan, lifespanUnit, 0, SECONDS);
    }

    /**
     * Associates the specified value with the specified key in this cache.
     * <p>
     * If the cache previously contained a mapping for the key, the old value is replaced by the
     * specified value.
     *<p>
     * If the return value of this operation will be ignored by the application,
     * the user is strongly encouraged to use the IGNORE_RETURN_VALUES Flag
     * when invoking this method in order to make it behave as efficiently
     * as possible (i.e. avoiding needless remote or network calls).
     *
     * \param key  key with which the specified value is to be associated
     * \param val  value value to be associated with the specified key
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     * \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \param maxIdleUnit TimeUnit for the maxIdle param
     *
     * \return the previous value associated with key, or NULL if there was no mapping for key.
     *
     */
    V* put(const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle, TimeUnit maxIdleUnit)
    {
        return (V *) base_put(&key, &val, toSeconds(lifespan, lifespanUnit), toSeconds(maxIdle, maxIdleUnit));
    }

    /**
     * Asynchronous version of put()
     * \param key  key with which the specified value is to be associated
     * \param val  value value to be associated with the specified key
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<V*> putAsync(const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0,
            std::function<V* (V*)> success = nullptr, std::function<V* (std::exception&)> fail = nullptr)
    {
        const K* pKey = &key;
        const V* pVal = &val;
        auto currTx = transactionManager.getCurrentTransaction();
        std::function<V* (void)> f = [=]
        {   return (V*)this->base_put(pKey, pVal, lifespan, maxIdle, currTx);};
        return goAsync(f, success, fail);
    }

    /**
     * Asynchronous version of put()
     * \param key  key with which the specified value is to be associated
     * \param val  value value to be associated with the specified key
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<V*> putAsync(const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit,
            std::function<V* (V*)> success = nullptr, std::function<V* (std::exception&)> fail = nullptr)
    {
        const K* pKey = &key;
        const V* pVal = &val;
        auto currTx = transactionManager.getCurrentTransaction();
        std::function<V* (void)> f = [=]
        {   return (V*)this->base_put(pKey, pVal, lifespan, lifespanUnit, currTx);};
        return goAsync(f, success, fail);
    }

    /**
     * Asynchronous version of put()
     * \param key  key with which the specified value is to be associated
     * \param val  value value to be associated with the specified key
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     * \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \param maxIdleUnit TimeUnit for the maxIdle param
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<V*> putAsync(const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle,
            TimeUnit maxIdleUnit, std::function<V* (V*)> success = nullptr, std::function<V* (std::exception&)> fail =
                    nullptr)
    {
        const K* pKey = &key;
        const V* pVal = &val;
        auto currTx = transactionManager.getCurrentTransaction();
        std::function<V* (void)> f = [=]
        { return (V*)this->base_put(pKey, pVal, toSeconds(lifespan, lifespanUnit), toSeconds(maxIdle, maxIdleUnit), currTx); };
        return goAsync(f, success, fail);
    }

    /**
     * Associates the specified value with the specified key in this cache if the specified key
     * is not already associated with some value. In such case key is associated with the given value.
     *
     * \param key  key with which the specified value is to be associated
     * \param val  value value to be associated with the specified key
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     *  \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* putIfAbsent(const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
        return putIfAbsent(key, val, lifespan, SECONDS, maxIdle, SECONDS);
    }
    /**
     * Associates the specified value with the specified key in this cache if the specified key
     * is not already associated with some value. In such case key is associated with the given value.
     *
     * \param key  key with which the specified value is to be associated
     * \param val  value value to be associated with the specified key
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* putIfAbsent(const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit)
    {
        return putIfAbsent(key, val, lifespan, lifespanUnit, 0, SECONDS);
    }
    /**
     * Associates the specified value with the specified key in this cache if the specified key
     * is not already associated with some value. In such case key is associated with the given value.
     *
     * \param key  key with which the specified value is to be associated
     * \param val  value value to be associated with the specified key
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     * \param maxIdle  the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \param maxIdleUnit TimeUnit for the maxIdle param
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* putIfAbsent(const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle,
            TimeUnit maxIdleUnit)
    {
        return (V *) base_putIfAbsent(&key, &val, toSeconds(lifespan, lifespanUnit), toSeconds(maxIdle, maxIdleUnit));
    }
    /**
     * Asynchronous version of putIfAbsentAsync()
     * \param key  key with which the specified value is to be associated
     * \param val  value value to be associated with the specified key
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<V*> putIfAbsentAsync(const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0,
            std::function<V* (V*)> success = nullptr, std::function<V* (std::exception&)> fail = nullptr)
    {
        const K* pKey = &key;
        const V* pVal = &val;
        auto currTx = transactionManager.getCurrentTransaction();
        std::function<V* (void)> f = [=]
        {   return (V*)this->base_putIfAbsent(pKey,pVal, lifespan, maxIdle, currTx);};
        return goAsync(f, success, fail);
    }
    /**
     * Asynchronous version of putIfAbsentAsync()
     * \param key  key with which the specified value is to be associated
     * \param val  value value to be associated with the specified key
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<V*> putIfAbsentAsync(const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit,
            std::function<V* (V*)> success = nullptr, std::function<V* (std::exception&)> fail = nullptr)
    {
        const K* pKey = &key;
        const V* pVal = &val;
        auto currTx = transactionManager.getCurrentTransaction();
        std::function<V* (void)> f = [=]
        { return (V*)this->base_putIfAbsent(pKey, pVal, toSeconds(lifespan, lifespanUnit), toSeconds(0, SECONDS), currTx); };
        return goAsync(f, success, fail);
    }
    /**
     * Asynchronous version of putIfAbsentAsync()
     * \param key  key with which the specified value is to be associated
     * \param val  value value to be associated with the specified key
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     * \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \param maxIdleUnit TimeUnit for the maxIdle param
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<V*> putIfAbsentAsync(const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit,
            uint64_t maxIdle, TimeUnit maxIdleUnit, std::function<V* (V*)> success = nullptr,
            std::function<V* (std::exception&)> fail = nullptr)
    {
        const K* pKey = &key;
        const V* pVal = &val;
        auto currTx = transactionManager.getCurrentTransaction();
        std::function<V* (void)> f = [=]
        { return (V*)this->base_putIfAbsent(pKey,pVal, toSeconds(lifespan, lifespanUnit), toSeconds(maxIdle, maxIdleUnit), currTx);};
        return goAsync(f, success, fail);
    }

    /**
     * Copies all of the mappings from the specified map to this cache. The effect of this call is equivalent to that of
     * calling put(k, v) on this cache once for each mapping from key k to value v in the specified map.
     * The behavior of this operation is undefined if the specified map is modified while the operation is in progress.
     *
     * \param map to be stored in this cache
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param maxIdle the maximum amount of time these object are allowed to be idle before they are considered as expired
     */
    void putAll(const std::map<K, V>& map, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
        putAll(map, lifespan, SECONDS, maxIdle, SECONDS);
    }

    /**
     * Copies all of the mappings from the specified map to this cache. The effect of this call is equivalent to that of
     * calling put(k, v) on this cache once for each mapping from key k to value v in the specified map.
     * The behavior of this operation is undefined if the specified map is modified while the operation is in progress.
     *
     * \param  map to be stored in this cache
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     *
     */
    void putAll(const std::map<K, V>& map, uint64_t lifespan, TimeUnit lifespanUnit)
    {
        putAll(map, lifespan, lifespanUnit, 0, SECONDS);
    }

    /**
     * Copies all of the mappings from the specified map to this cache. The effect of this call is equivalent to that of
     * calling put(k, v) on this cache once for each mapping from key k to value v in the specified map.
     * The behavior of this operation is undefined if the specified map is modified while the operation is in progress.
     *
     * \param  map to be stored in this cache
     * \param lifespan the lifespan of this entry. A negative value is interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     * \param maxIdle the maximum amount of time these object are allowed to be idle before they are considered as expired
     * \param maxIdleUnit TimeUnit for the maxIdle param
     *
     */
    void putAll(const std::map<K, V>& map, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle,
            TimeUnit maxIdleUnit)
    {
        uint64_t lifespanMillis = toSeconds(lifespan, lifespanUnit);
        uint64_t maxIdleMillis = toSeconds(maxIdle, maxIdleUnit);
        std::map<const void*, const void*> tmpMap;
        for (auto& it : map)
        {
            tmpMap[(const void *)&it.first] = (const void *)&it.second;
        }
        base_putAll(tmpMap, lifespanMillis, maxIdleMillis);
    }

    /**
     * Asynchronous version of putAll()
     *    See the synchronous doc for parameters not explained here
     * \param  map to be stored in this cache
     * \param lifespan the lifespan of these entries. A negative value is interpreted as unlimited lifespan
     * \param maxIdle the maximum amount of time these objects are allowed to be idle before they are considered as expired
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<void> putAllAsync(const std::map<K, V>& map, uint64_t lifespan = 0, uint64_t maxIdle = 0,
            std::function<void()> success = nullptr, std::function<void(std::exception&)> fail = nullptr)
    {
        auto pMap = map;
        auto f = [=]
        {   return this->putAll(pMap, lifespan, maxIdle);};
        return goAsync(f, success, fail);
    }

    /**
     * Asynchronous version of putAll()
     *    See the synchronous doc for parameters not explained here
     * \param  map to be stored in this cache
     * \param lifespan the lifespan of these entries. A negative value is interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<void> putAllAsync(const std::map<K, V>& map, uint64_t lifespan, TimeUnit lifespanUnit,
            std::function<void(void)> success = nullptr, std::function<void(std::exception&)> fail = nullptr)
    {
        auto pMap = map;
        auto f = [=]
        {   this->putAll(pMap, lifespan, lifespanUnit);};
        return goAsync(f, success, fail);
    }

    /**
     * Asynchronous version of putAll()
     *    See the synchronous doc for parameters not explained here
     * \param  map to be stored in this cache
     * \param lifespan the lifespan of these entries. A negative value is interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     * \param maxIdle the maximum amount of time these are allowed to be idle before they are considered as expired
     * \param maxIdleUnit TimeUnit for the maxIdle param
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<void> putAllAsync(const std::map<K, V>& map, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle,
            TimeUnit maxIdleUnit, std::function<void(void)> success = nullptr,
            std::function<void(std::exception&)> fail = nullptr)
    {
        auto pMap = &map;
        auto f = [=]
        {   this->putAll(*pMap,lifespan, lifespanUnit, maxIdle, maxIdleUnit);};
        return goAsync(f, success, fail);
    }

    /**
     * Replaces the entry for a key with a given new value.
     *
     * \param key  key with which the specified value is to be associated
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan
     * \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     *
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* replace(const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
        return replace(key, val, lifespan, SECONDS, maxIdle, SECONDS);
    }
    /**
     * Replaces the entry for a key with a given new value.
     *
     * \param key  key with which the specified value is to be associated
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     *
     *
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* replace(const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit)
    {
        return replace(key, val, lifespan, lifespanUnit, 0, SECONDS);
    }
    /**
     * Replaces the entry for a key with a given new value.
     *
     * \param key  key with which the specified value is to be associated
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     * \param maxIdle  the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \param maxIdleUnit TimeUnit for the maxIdle param
     *
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* replace(const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle,
            TimeUnit maxIdleUnit)
    {
        return (V *) base_replace(&key, &val, toSeconds(lifespan, lifespanUnit), toSeconds(maxIdle, maxIdleUnit));
    }
    /**
     * Replaces the entry for a key with a new value only if currently mapped to a given old value.
     *
     * \param key  key with which the specified value is to be associated
     * \param oldVal  value expected to be associated with the specified key
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan
     * \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     *
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* replace(const K& key, const V& oldVal, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
        return replace(key, oldVal, val, lifespan, SECONDS, maxIdle, SECONDS);
    }
    /**
     * Replaces the entry for a key with a new value only if currently mapped to a given old value.
     *
     * \param key  key with which the specified value is to be associated
     * \param oldVal  value expected to be associated with the specified key
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     *
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* replace(const K& key, const V& oldVal, const V& val, uint64_t lifespan, TimeUnit lifespanUnit)
    {
        return replace(key, oldVal, val, lifespan, lifespanUnit, 0, SECONDS);
    }
    /**
     * Replaces the entry for a key with a new value only if currently mapped to a given old value.
     *
     * \param key  key with which the specified value is to be associated
     * \param oldVal  value expected to be associated with the specified key
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     * \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \param maxIdleUnit TimeUnit for the maxIdle param
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* replace(const K& key, const V& oldVal, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle,
            TimeUnit maxIdleUnit)
    {
        throw UnsupportedOperationException();
    }

    /**
     * Asynchronous version of replace()
     * \param key  key with which the specified value is to be associated
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan
     *  \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<V*> replaceAsync(const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0,
            std::function<V* (V*)> success = nullptr, std::function<V* (std::exception&)> fail = nullptr)
    {
        auto pKey = &key, pVal = &val;
        auto f = [=]
        {   return this->replace(*pKey, *pVal, lifespan, SECONDS, maxIdle, SECONDS);};
        return goAsync(f, success, fail);
    }

    /**
     * Asynchronous version of replace()
     * \param key  key with which the specified value is to be associated
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan
     *  \param lifespanUnit TimeUnit for the lifespan param
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<V*> replaceAsync(const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit,
            std::function<V* (V*)> success = nullptr, std::function<V* (std::exception&)> fail = nullptr)
    {
        auto pKey = &key, pVal = &val;
        auto f = [=]
        {   return this->replace(*pKey, *pVal, lifespan, lifespanUnit, 0, SECONDS);};
        return goAsync(f, success, fail);
    }

    /**
     * Asynchronous version of replace()
     * \param key  key with which the specified value is to be associated
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan
     *  \param lifespanUnit TimeUnit for the lifespan param
     *  \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \param maxIdleUnit TimeUnit for the maxIdle param
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<V*> replaceAsync(const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle,
            TimeUnit maxIdleUnit, std::function<V* (V*)> success = nullptr, std::function<V* (std::exception&)> fail =
                    nullptr)
    {
        auto pKey = &key, pVal = &val;
        auto f = [=]
        {   return this->replace(*pKey, *pVal, lifespan, lifespanUnit, maxIdle, maxIdleUnit);};
        return goAsync(f, success, fail);
    }

    /**
     * Asynchronous version of replace()
     *    See the synchronous doc for parameters not explained here
     * \param key  key with which the specified value is to be associated
     * \param oldVal  value expected to be associated with the specified key
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan
     * \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<V*> replaceAsync(const K& key, const V& oldVal, const V& val, uint64_t lifespan = 0, uint64_t maxIdle =
            0, std::function<V* (V*)> success = nullptr, std::function<V* (std::exception&)> fail = nullptr)
    {
        auto f = [=]
        {   return this->replace(key, oldVal, val, lifespan, SECONDS, maxIdle, SECONDS);};
        return goAsync(f, success, fail);
    }

    /**
     * Asynchronous version of replace()
     *    See the synchronous doc for parameters not explained here
     * \param key  key with which the specified value is to be associated
     * \param oldVal  value expected to be associated with the specified key
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan
     * \param lifespanUnit TimeUnit for the lifespan param
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<V*> replaceAsync(const K& key, const V& oldVal, const V& val, uint64_t lifespan, TimeUnit lifespanUnit,
            std::function<V* (V*)> success = nullptr, std::function<V* (std::exception&)> fail = nullptr)
    {
        auto f = [=]
        {   return this->replace(key, oldVal, val, lifespan, lifespanUnit, 0, SECONDS);};
        return goAsync(f, success, fail);
    }

    /**
     * Removes key/value pair from the cache given a key. Optionally return a value stored
     * under the given key
     *
     * \param key the of the entry to be removed
     * \return NULL or value stored under this key
     */
    V* remove(const K& key)
    {
        return (V *) base_remove(&key);
    }

    /**
     * Asynchronous version of removeWithVersion()
     *    See the synchronous doc for parameters not explained here
     * \param key the of the entry to be removed
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a bool. True if the entry has been removed
     *
     */
    std::future<V*> removeAsync(const K& key, std::function<V* (V*)> success = nullptr,
            std::function<V* (std::exception&)> fail = nullptr)
    {
        auto f = [=]
        {   return this->remove(key);};
        return goAsync(f, success, fail);
    }

    /**
     * Returns true if this cache contains a key/value pair where the key is equal to a specified key.
     *
     * \param key the key
     * \return true if such key is present in this cache
     */
    bool containsKey(const K& key)
    {
        return base_containsKey(&key);
    }

    /**
     * Unsupported operation in this release of Hot Rod client. UnsupportedOperationException is
     * thrown if his method is invoked.
     */
    bool containsValue(const V& val)
    {
        throw UnsupportedOperationException();
    }
    /**
     * Replaces the given value only if its version matches the supplied version.
     *
     *
     * \param key the key
     * \param val the new value for the given key
     * \param version numeric version that should match the one in the server
     *                for the operation to succeed
     * \param lifespan the new lifespan for this key/value entry pair
     * \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \return true if the value has been replaced
     *
     * \sa VersionedValue
     */
    bool replaceWithVersion(const K& key, const V& val, uint64_t version, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
        return base_replaceWithVersion(&key, &val, version, lifespan, maxIdle);
    }

    /**
     * Asynchronous version of replaceWithVersion()
     * \param key the key
     * \param val the new value for the given key
     * \param version numeric version that should match the one in the server
     *                for the operation to succeed
     * \param lifespan the new lifespan for this key/value entry pair
     * \param maxIdle the maximum amount of time this entry is allowed to be idle before it is considered as expired
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a pointer to the previous value stored in the cache for the given key or null
     *
     */
    std::future<bool> replaceWithVersionAsync(const K& key, const V& val, uint64_t version, uint64_t lifespan,
            uint64_t maxIdle, std::function<bool(bool)> success = nullptr, std::function<bool(std::exception&)> fail =
                    nullptr)
    {
        auto f = [=]
        {   return this->replaceWithVersion(key, val, version, lifespan, maxIdle);};
        return goAsync(f, success, fail);
    }

    /**
     * Removes the given entry only if its version matches the supplied version.
     *
     * A typical use case looks like this:
     * <pre>
     * std::pair<std::shared_ptr<std::string>, VersionedValue> rv = remoteCache.getWithVersioned(key);
     * //some processing...
     * remoteCache.removeWithVersion(key, rv.second().version);
     * </pre>
     * Last call (removeWithVersion) will make sure that the entry will only be removed if it
     * hasn't been changed in the meantime.
     *
     * \param key key
     * \param version version of entry to remove
     * \return true if the entry has been removed
     * \sa VersionedValue
     *
     */
    bool removeWithVersion(const K& key, uint64_t version)
    {
        return base_removeWithVersion(&key, version);
    }

    /**
     * Asynchronous version of removeWithVersion()
     * \param key key
     * \param version version of entry to remove
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a bool. True if the entry has been removed
     *
     */
    std::future<bool> removeWithVersionAsync(const K& key, uint64_t version,
            std::function<bool(bool)> success = nullptr, std::function<bool(std::exception&)> fail = nullptr)
    {
        auto f = [=]
        {   return this->removeWithVersion(key, version);};
        return goAsync(f, success, fail);
    }

    /**
     *  Returns the std::pair with value and VersionedValue associated with
     *  the supplied key parameter or a default initialized std::pair if it
     *  doesn't exist
     *
     * \param key the key
     * \return std::pair where first entry is a pointer to a value and
     *  second pair entry is an accompanying VersionedValue
     *
     */
    std::pair<std::shared_ptr<V>, VersionedValue> getWithVersion(const K& key)
    {
        VersionedValue version;
        void *value = base_getWithVersion(&key, &version);
        return std::make_pair(std::shared_ptr<V>((V *) value), version);
    }
    /**
     * Returns the std::pair with value and MetadataValue associated to the
     * supplied key parameter or a default initialized std::pair if it
     * doesn't exist
     *
     * \param key the key
     * \return std::pair where first entry is a pointer to a value and
     *  second pair entry is an accompanying MetadataValue
     *
     */
    std::pair<std::shared_ptr<V>, MetadataValue> getWithMetadata(const K& key)
    {
        MetadataValue metadata;
        void *value = base_getWithMetadata(&key, &metadata);
        return std::make_pair(std::shared_ptr<V>((V *) value), metadata);
    }
    /**
     * Unsupported operation in this release of Hot Rod client. UnsupportedOperationException is
     * thrown if his method is invoked.
     */
    std::map<std::shared_ptr<K>, std::shared_ptr<V> > getBulk()
    {
        return getBulk(0);
    }
    /**
     * Returns nrOfEntries from a remote cache in map.
     *TODO
     */
    std::map<std::shared_ptr<K>, std::shared_ptr<V> > getBulk(int nrOfEntries)
    {
        std::map<void*, void*> mbuf;
        std::map<std::shared_ptr<K>, std::shared_ptr<V> > result;
        base_getBulk(nrOfEntries, mbuf);
        for (auto it = mbuf.begin(); it != mbuf.end(); ++it)
        {
            result[std::shared_ptr<K>((K*) it->first)] = std::shared_ptr<V>((V*) it->second);
        }
        return result;
    }
    /**
     * Execute script on server
     * \param name name of the script
     * \param args maps of (name,value) arguments
     * \return byte[] result in dark matter shape
     */
    std::vector<unsigned char> execute(const std::string& name, const std::map<std::string, std::string>& args)
    {
        return base_execute(name, args);
    }

    /**
     * Execute script on server
     * \param name name of the script
     * \param args maps of (name,value) arguments
     * \return byte[] result in dark matter shape
     */
    std::vector<unsigned char> execute(const std::string& name, const std::map<std::vector<char>, std::vector<char> >& args)
    {
    	return base_execute(name,args);
    }

    /**
     * Execute script on server
     * \param name name of the script
     * \param args maps of (name,value) arguments
     * \param key a key used to target a specific node in the topology
     * \return byte[] result in dark matter shape
     */
    std::vector<unsigned char> execute(const std::string& name, const std::map<std::string, std::string>& args, const K& key)
    {
        return base_execute(&key, name, args);
    }

    /**
     * Execute script on server
     * \param name name of the script
     * \param args maps of (name,value) arguments
     * \param key a key used to target a specific node in the topology
     * \return byte[] result in dark matter shape
     */
    std::vector<unsigned char> execute(const std::string& name, const std::map<std::vector<char>, std::vector<char> >& args, const K& key)
    {
    	return base_execute(&key, name, args);
    }

    /**
     * Execute a query on server
     * \param qr the QueryRequest oject
     * \return a QueryResponse object with the query result
     */
    QueryResponse query(const QueryRequest &qr)
    {
        return base_query(qr);
    }

    /**
     * Execute a query on server
     * \param qr the query string
     * \param size the length of the query string
     * \return byte[] result in dark matter shape
     */
    std::vector<unsigned char> query(std::vector<unsigned char> qr, size_t size)
    {
        return base_query_char(qr, size);
    }

    /**
     * Unsupported operation in this release of Hot Rod client. UnsupportedOperationException is
     * thrown if his method is invoked.
     */
    std::set<std::pair<K, V> > entrySet()
    {
        throw UnsupportedOperationException();
    }
    /**
     * Returns all keys in the remote server.  It'll invoke a command over the network each time this method is called.
     * If the remote cache is a distributed cache, it will retrieve all of the keys from all nodes in the cluster.
     * Please use with care for cache with large data set.
     *
     * \return a std::set of pointers to all keys in this given cache
     */
    std::set<std::shared_ptr<K> > keySet()
    {
        std::vector<void*> p;
        base_keySet(0, p);

        std::set<std::shared_ptr<K> > result;
        for (size_t i = 0; i < p.size(); ++i)
        {
            result.insert(std::shared_ptr<K>((K*) p.data()[i]));
        }
        return result;
    }
    /**
     *Returns an approximate number of key/value pairs in this cache.
     *
     *\return number of entries in cache
     */
    uint64_t size()
    {
        return base_size();
    }
    /**
     *Returns true if and only if this cache has no entries.
     *
     *\return true if cache is empty
     */
    bool isEmpty()
    {
        return 0 == size();
    }
    /**
     * Unsupported operation in this release of Hot Rod client. UnsupportedOperationException is
     * thrown if his method is invoked.
     */
    std::vector<V> values()
    {
        throw UnsupportedOperationException();
    }
    /**
     * Returns statistics for this cache.
     *
     * \return a std::map of string key/value pairs where each key is a certain statistic property
     * while the matching value represents the value for that property
     */
    std::map<std::string, std::string> stats()
    {
        std::map<std::string, std::string> statistics;
        base_stats(statistics);
        return statistics;
    }
    /**
     * Clears all entries in this cache
     *
     *
     */
    void clear()
    {
        base_clear();
    }

    /**
     * Asynchronous version of clear()
     *    See the synchronous doc for parameters not explained here
     * \param success function to be executed on success
     * \param fail function to be executed if exceptions occur
     * \return a future containing a bool. True if the entry has been removed
     *
     */
    std::future<void> clearAsync(std::function<void(void)> success = nullptr,
            std::function<void(std::exception&)> fail = nullptr)
    {
        auto f = [=]
        {   this->clear();};
        return goAsync(f, success, fail);
    }

    /**
     * Pings remote cache on a Hot Rod server
     *
     *
     */
    void ping()
    {
        base_ping();
    }

    /**
     * Start client listener and register it on the server
     *
     * \param clientListener object defining the listener, filter and converter and the callback funcs
     * \param filterFactoryParams parameters for optional server filter setup
     * \param converterFactoryParams parameter for optional server converter setup
     * \param recoveryCallback function to be called if transport goes down
     * \return
     *
     */
    void addClientListener(ClientListener& clientListener, std::vector<std::vector<char> > filterFactoryParams,
            std::vector<std::vector<char> > converterFactoryParams, const std::function<void()> &recoveryCallback =
                    nullptr)
    {
        base_addClientListener(clientListener, filterFactoryParams, converterFactoryParams, recoveryCallback);
    }

    /**
     * Stop and remove client listener
     *
     * \param clientListener the listener to be removed
     * \return
     *
     */
    void removeClientListener(ClientListener& clientListener)
    {
        base_removeClientListener(clientListener);
    }

#if !defined(SWIG) && !defined(SWIGCSHARP)
    /**
     * Start a client listener on the specified query and register it on the server
     *
     * \param cql object defining the query to be observed
     **/
    void addContinuousQueryListener(ContinuousQueryListener<K, V>& cql)
    {
        base_addContinuousQueryListener(cql);
    }
    template<typename ... Params>
    /**
     * Start a client listener on a query that returns projection or aggregate and register it on the server
     *
     * \param cql object defining the query to be observed
     **/
    void addContinuousQueryListener(ContinuousQueryListener<K, V, Params...>& cql)
    {
        base_addContinuousQueryListener(cql);
    }

    /**
     * Start a client listener on the specified query and register it on the server
     *
     * \param cql object defining the query to be observed
     **/
    template<typename ... Params>
    void removeContinuousQueryListener(ContinuousQueryListener<Params...>& cql)
    {
        base_removeClientListener(cql.cl);
    }
#endif

    CacheTopologyInfo getCacheTopologyInfo()
    {
        return base_getCacheTopologyInfo();
    }
    /**
     * Applies one or more Flag enums to the scope of a single invocation.
     *
     * \param flags the flags to apply to an invocation
     * \return the current RemoteCache instance
     */
    RemoteCache<K, V>& withFlags(Flag flags)
    {
        base_withFlags(flags);
        return *this;
    }

    /**
     * Get a remote execution context
     *
     * \return a RemoteExecution object that simplify the call of a remote execution task
     *
     */
    template <class M = JBossMarshaller>
    RemoteExecution<V, M> getRemoteExecution()
    {
        return RemoteExecution<V,M>(*this);
    }

    RemoteCache(const RemoteCache &other) :
            RemoteCacheBase(other), keyMarshaller(other.keyMarshaller), valueMarshaller(other.valueMarshaller)
    {
        setMarshallers(this, &keyMarshall, &valueMarshall, &keyUnmarshall, &valueUnmarshall);
        valueCopyConstructor = [](const void* src) { V* v = new V(*static_cast<const V*>(src)); return (void*) v; };
        valueDestructor = [](const void* obj) { delete static_cast<const V*>(obj); };
    }

    /**
     * Create a new RemoteCache instance with the specified DataFormat
     *
     * \param df the DataFormat struct specifying entry media type and marshaller
     *
     * \return the new RemoteCache that can handle the specified DataFormat
     */
    RemoteCache withDataFormat(DataFormat<K,V>* df) {
        RemoteCache rc(*this);
        rc.keyMarshaller = (df->keyMarshaller != nullptr) ? df->keyMarshaller : this->keyMarshaller;
        rc.valueMarshaller = (df->valueMarshaller != nullptr) ? df->valueMarshaller : this->valueMarshaller;
        rc.cloneImplWithDataFormat(df);
        return rc;
    }

    /**
     * Create a new RemoteCache instance with the specified DataFormat
     *
     * Use this method if the new cache must deal with different key,value types than those defined for this cache
     *
     * \param df the DataFormat struct specifying entry media type and marshaller
     *
     * \return the new RemoteCache that can handle the specified DataFormat
     */
    template <class K1, class V1> RemoteCache<K1,V1> withDataFormat(DataFormat<K1,V1>* df) {
        RemoteCache<K1,V1> rc((RemoteCacheBase&)*this);
        rc.keyMarshaller = df->keyMarshaller;
        rc.valueMarshaller = df->valueMarshaller;
        rc.cloneImplWithDataFormat(df);
    return rc;
    }

protected:

    RemoteCache(const RemoteCacheBase& rcb) : RemoteCacheBase(rcb){
        setRemoteCachePtr(this);
    }

    RemoteCache(TransactionManager& tm, TransactionTable& tt, bool forceReturnValue, bool transactional) :
            RemoteCacheBase(tm, tt, forceReturnValue, transactional)
    {
        valueCopyConstructor = [](const void* src) { V* v = new V(*static_cast<const V*>(src)); return (void*) v; };
        valueDestructor = [](const void* obj) { delete static_cast<const V*>(obj); };
        setMarshallers(this, &keyMarshall, &valueMarshall, &keyUnmarshall, &valueUnmarshall);
    }

private:
    uint64_t toSeconds(uint64_t time, TimeUnit unit)
    {
        uint64_t result;
        switch (unit)
        {
        case NANOSECONDS:
            result = (uint64_t) ceil(time / 1000000000.0);
            break;
        case MICROSECONDS:
            result = (uint64_t) ceil(time / 1000000.0);
            break;
        case MILLISECONDS:
            result = (uint64_t) ceil(time / 1000.0);
            break;
        case SECONDS:
            result = time;
            break;
        case MINUTES:
            result = time * 60;
            break;
        case HOURS:
            result = time * 3600;
            break;
        case DAYS:
            result = time * 86400;
            break;
        default:
            std::stringstream ss;
            ss << "Unhandled TimeUnit specified: " << unit << ".";
            throw std::invalid_argument(ss.str());
        }
        return result;
    }

    // type-hiding and resurrecting support
    static void keyMarshall(void *thisp, const void* key, std::vector<char> &buf)
    {
        ((RemoteCache<K, V> *) thisp)->keyMarshaller->marshall(*(const K *) key, buf);
    }
    static void valueMarshall(void* thisp, const void* val, std::vector<char> &buf)
    {
        ((RemoteCache<K, V> *) thisp)->valueMarshaller->marshall(*(const V *) val, buf);
    }
    static void* keyUnmarshall(void *thisp, const std::vector<char> &buf)
    {
        return ((RemoteCache<K, V> *) thisp)->keyMarshaller->unmarshall(buf);
    }
    static void* valueUnmarshall(void* thisp, const std::vector<char> &buf)
    {
        return ((RemoteCache<K, V> *) thisp)->valueMarshaller->unmarshall(buf);
    }

    std::shared_ptr<Marshaller<K> > keyMarshaller;
    std::shared_ptr<Marshaller<V> > valueMarshaller;

    friend class RemoteCacheManager;
    template <class K1, class V1> friend class RemoteCache;
};
}
} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHE_H */
