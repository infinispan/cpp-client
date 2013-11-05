#ifndef ISPN_HOTROD_REMOTECACHE_H
#define ISPN_HOTROD_REMOTECACHE_H



#include "infinispan/hotrod/RemoteCacheBase.h"
#include "infinispan/hotrod/Marshaller.h"
#include "infinispan/hotrod/Flag.h"
#include "infinispan/hotrod/MetadataValue.h"
#include "infinispan/hotrod/TimeUnit.h"
#include "infinispan/hotrod/VersionedValue.h"

#include <cmath>
#include <set>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace infinispan {
namespace hotrod {

/**
 * Provides remote reference to a cache residing on a Hot Rod server/cluster.
 *
 * %RemoteCache is intended to be similar to Infinispan Java %RemoteCache interface as much as possible. Just
 * like %RemoteCache interface in its Java HotRod counterpart %RemoteCache features:
 *
 * <p/>
 * <b>New methods</b>: Although some methods are very simila to Java Map interface, %RemoteCache also adds new
 * methods to optimize/reduce network traffic: e.g. versioned put operation.
 * <p/>
 * <b>Concurrency</b>: implementations will support multi-threaded access.
 * <p/>
 * <b>Return values</b>: previously existing values for certain methods are not returned, NULL
 * is returned instead unless method is using fluent variant withFlags (see below).
 * <p/>
 * <b>Synthetic methods</b>: aggregate methods are being implemented based on other Hot Rod %operations. For example,
 * putAll method is implemented using  multiple individual puts. Therefore these methods are not atomic and that they are
 * costly, e.g. as the number of network round-trips is not one, but the size of the added map.
 * <p/>
 * It is possible change the default method behavior using Flag enum. For example:
 * <pre>
 *      %RemoteCache cache = ...;
 *      std::auto_ptr<std::string> rv(cache.withFlags(FORCE_RETURN_VALUE).put(k,v));
 * </pre>
 *
 * In the previous example Flag enum FORCE_RETURN_VALUE will make the client to also return previously existing value
 * associated with <tt>k</tt> key. If this flag would not be present, %RemoteCache would return (by default) <tt>NULL</tt>.
 * This is in order to avoid fetching a possibly large object from the remote server, which might not be needed.
 * <p/>
 *
 */
template <class K, class V> class RemoteCache : private RemoteCacheBase
{
  public:
    /**
     * Retrieves the name of the cache
     *
     * \return the name of the cache
     */
    std::string getName() {
	throw UnsupportedOperationException();
    }

    /**
     * Retrieves the version of Infinispan
     *
     * \return a version string
     */
    std::string getVersion() {
	throw UnsupportedOperationException();
    }

    /**
     * Retrieves the protocol version
     *
     * \return protocol version as string
     */
    std::string getProtocolVersion() {
	throw UnsupportedOperationException();
    }

    /**
     * Returns the value to which the specified key is mapped, or NULL if this cache contains no mapping for the key.
     *
     * \param key the key whose associated value is to be returned
     * \return the value to which the specified key is mapped, or NULL if this map contains no mapping for the key
     *
     */
    V* get(const K& key) {
        ScopedBuffer vbuf;
        base_get(&key, &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
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
     * \param value  value value to be associated with the specified key
     *
     * \return the previous value associated with key, or NULL if there was no mapping for key.
     *
     */
    V* put(
        const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
	return put(key, val, lifespan, SECONDS, maxIdle, SECONDS);
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
     * \param value  value value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     *  \param lifespanUnit  unit of measurement for the lifespan
     *
     * \return the previous value associated with key, or NULL if there was no mapping for key.
     *
     */
    V* put(
        const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit)
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
     * \param value  value value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     *  \param lifespanUnit  unit of measurement for the lifespan
     *  \param maxIdle  the maximum amount of time this key is allowed to be idle for before it is considered as
     *                        expired
     * \param maxIdleUnit  time unit for max idle time
     *
     * \return the previous value associated with key, or NULL if there was no mapping for key.
     *
     */
    V* put(
	const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle, TimeUnit maxIdleUnit)
    {
	ScopedBuffer vbuf;
        base_put(&key, &val, toSeconds(lifespan, lifespanUnit), toSeconds(maxIdle, maxIdleUnit), &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }
    /**
     * Associates the specified value with the specified key in this cache if the specified key
     * is not already associated with some value. In such case key is associated with the given value.
     *
     * \param key  key with which the specified value is to be associated
     * \param value  value value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     *  \param maxIdle  the maximum amount of time this key is allowed to be idle for before it is considered as
     *                        expired
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* putIfAbsent(
        const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
	return putIfAbsent(key, val, lifespan, SECONDS, maxIdle, SECONDS);
    }
    /**
     * Associates the specified value with the specified key in this cache if the specified key
     * is not already associated with some value. In such case key is associated with the given value.
     *
     * \param key  key with which the specified value is to be associated
     * \param value  value value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     * \param lifespanUnit  unit of measurement for the lifespan
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* putIfAbsent(
		   const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit)
    {
	return putIfAbsent(key, val, lifespan, lifespanUnit, 0, SECONDS);
    }
    /**
     * Associates the specified value with the specified key in this cache if the specified key
     * is not already associated with some value. In such case key is associated with the given value.
     *
     * \param key  key with which the specified value is to be associated
     * \param value  value value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     *  \param lifespanUnit  unit of measurement for the lifespan
     *  \param maxIdle  the maximum amount of time this key is allowed to be idle for before it is considered as
     *                        expired
     * \param maxIdleUnit  time unit for max idle time
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* putIfAbsent(
		   const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle, TimeUnit maxIdleUnit)
    {
	ScopedBuffer vbuf;
        base_putIfAbsent(&key, &val, toSeconds(lifespan, lifespanUnit), toSeconds(maxIdle, maxIdleUnit), &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }
    /**
     * Copies all of the mappings from the specified map to this cache. The effect of this call is equivalent to that of
     * calling put(k, v) on this cache once for each mapping from key k to value v in the specified map.
     * The behavior of this operation is undefined if the specified map is modified while the operation is in progress.
     *
     * \param  map to be stored in this cache
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     * \param maxIdle  the maximum amount of time this key is allowed to be idle for before it is considered as
     *                        expired
     */
    void putAll(const std::map<K, V>& map, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
	return putAll(map, lifespan, SECONDS, maxIdle, SECONDS);
    }
    /**
     * Copies all of the mappings from the specified map to this cache. The effect of this call is equivalent to that of
     * calling put(k, v) on this cache once for each mapping from key k to value v in the specified map.
     * The behavior of this operation is undefined if the specified map is modified while the operation is in progress.
     *
     * \param  map to be stored in this cache
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     * \param lifespanUnit  unit of measurement for the lifespan
     *
     */
    void putAll(const std::map<K, V>& map, uint64_t lifespan, TimeUnit lifespanUnit)
    {
	return putAll(map, lifespan, lifespanUnit, 0, SECONDS);
    }
    /**
     * Copies all of the mappings from the specified map to this cache. The effect of this call is equivalent to that of
     * calling put(k, v) on this cache once for each mapping from key k to value v in the specified map.
     * The behavior of this operation is undefined if the specified map is modified while the operation is in progress.
     *
     * \param  map to be stored in this cache
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     * \param lifespanUnit  unit of measurement for the lifespan
     * \param maxIdle  the maximum amount of time this key is allowed to be idle for before it is considered as
     *                        expired
     * \param maxIdleUnit  time unit for max idle time
     *
     */
    void putAll(const std::map<K, V>& map, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle, TimeUnit maxIdleUnit)
    {
	uint64_t lifespanMillis = toSeconds(lifespan, lifespanUnit);
	uint64_t maxIdleMillis = toSeconds(maxIdle, maxIdleUnit);

	for (typename std::map<K, V>::const_iterator it = map.begin(); it != map.end(); ++it) {
	    put(it->first, it->second, lifespanMillis, maxIdleMillis);
	}
    }
    /**
     * Replaces the entry for a key with a given new value.
     *
     * \param key  key with which the specified value is to be associated
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     * \param maxIdle  the maximum amount of time this key is allowed to be idle for before it is considered as
     *                        expired
     *
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* replace(
	       const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
	return replace(key, val, lifespan, SECONDS, maxIdle, SECONDS);
    }
    /**
     * Replaces the entry for a key with a given new value.
     *
     * \param key  key with which the specified value is to be associated
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     * \param lifespanUnit  unit of measurement for the lifespan
     *
     *
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* replace(
	       const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit)
    {
	return replace(key, val, lifespan, lifespanUnit, 0, SECONDS);
    }
    /**
     * Replaces the entry for a key with a given new value.
     *
     * \param key  key with which the specified value is to be associated
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     * \param lifespanUnit  unit of measurement for the lifespan
     * \param maxIdle  the maximum amount of time this key is allowed to be idle for before it is considered as
     *                        expired
     * \param maxIdleUnit  time unit for max idle time
     *
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* replace(
	       const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle, TimeUnit maxIdleUnit)
    {
        ScopedBuffer vbuf;
        base_replace(&key, &val, toSeconds(lifespan, lifespanUnit), toSeconds(maxIdle, maxIdleUnit), &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }
    /**
     * Replaces the entry for a key with a new value only if currently mapped to a given old value.
     *
     * \param key  key with which the specified value is to be associated
     * \param oldVal  value expected to be associated with the specified key
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     * \param maxIdle  the maximum amount of time this key is allowed to be idle for before it is considered as
     *                        expired
     *
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* replace(
	       const K& key, const V& oldVal, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
	return replace(key, oldVal, val, lifespan, SECONDS, maxIdle, SECONDS);
    }
    /**
     * Replaces the entry for a key with a new value only if currently mapped to a given old value.
     *
     * \param key  key with which the specified value is to be associated
     * \param oldVal  value expected to be associated with the specified key
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     * \param lifespanUnit  unit of measurement for the lifespan
     *
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* replace(
	       const K& key, const V& oldVal, const V& val, uint64_t lifespan, TimeUnit lifespanUnit)
    {
	return replace(key, oldVal, val, lifespan, lifespanUnit, 0, SECONDS);
    }
    /**
     * Replaces the entry for a key with a new value only if currently mapped to a given old value.
     *
     * \param key  key with which the specified value is to be associated
     * \param oldVal  value expected to be associated with the specified key
     * \param val  value to be associated with the specified key
     * \param lifespan  lifespan of the entry.  Negative values are interpreted as unlimited lifespan.
     *  \param lifespanUnit  unit of measurement for the lifespan
     *  \param maxIdle  the maximum amount of time this key is allowed to be idle for before it is considered as
     *                        expired
     * \param maxIdleUnit  time unit for max idle time
     * \return the previous value associated with the specified key, or NULL if there was no mapping for the key.
     */
    V* replace(
	       const K& key, const V& oldVal, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle, TimeUnit maxIdleUnit)
    {
	throw UnsupportedOperationException();
    }

    /**
      * Removes key/value pair from the cache given a key. Optionally return a value stored
      * under the given key
      *
      *\return NULL or value stored under this key
      */
    V* remove(const K& key) {
        ScopedBuffer vbuf;
        base_remove(&key, &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }
    /**
     * Returns true if this cache contains a key/value pair where the key is equal to a specified key.
     *
     *\return true if such key is present in this cache
     */
    bool containsKey(const K& key) {
        bool res;
        base_containsKey(&key, &res);
        return res;
    }
    /**
     * Unsupported operation in this release of Hot Rod client. UnsupportedOperationException is
     * thrown if his method is invoked.
     */
    bool containsValue(const V& val) {
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
     * \param maxIdle the maxIdle for this key/value entry pair
     * \return true if the value has been replaced
     *
     * \sa VersionedValue
     */
    bool replaceWithVersion(
        const K& key, const V& val,
        uint64_t version, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
        bool res;
        base_replaceWithVersion(&key, &val, version, lifespan, maxIdle, &res);
        return res;
    }
    /**
     * Removes the given entry only if its version matches the supplied version.
     *
     * A typical use case looks like this:
     * <pre>
     * std::pair<HR_SHARED_PTR<std::string>, VersionedValue> rv = remoteCache.getWithVersioned(key);
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
    bool removeWithVersion(const K& key, uint64_t version) {
        bool res;
        base_removeWithVersion(&key, version, &res);
        return res;
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
    std::pair<HR_SHARED_PTR<V>, VersionedValue> getWithVersion(const K& key) {
        ScopedBuffer vbuf;
        VersionedValue version;
        base_getWithVersion(&key, &vbuf, &version);
        return vbuf.getBytes() ?
                    std::make_pair(HR_SHARED_PTR<V>(valueMarshaller->unmarshall(vbuf)), version) :
                    std::make_pair(HR_SHARED_PTR<V>(), version);
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
    std::pair<HR_SHARED_PTR<V>, MetadataValue> getWithMetadata(const K& key) {
        ScopedBuffer vbuf;
        MetadataValue metadata;
        base_getWithMetadata(&key, &vbuf, &metadata);
        return vbuf.getBytes() ?
            std::make_pair(HR_SHARED_PTR<V>(valueMarshaller->unmarshall(vbuf)), metadata) :
            std::make_pair(HR_SHARED_PTR<V>(), metadata);
    }
    /**
     * Unsupported operation in this release of Hot Rod client. UnsupportedOperationException is
     * thrown if his method is invoked.
     */
    std::map<HR_SHARED_PTR<K>, HR_SHARED_PTR<V> > getBulk() {
	throw UnsupportedOperationException();
    }
    /**
     * Returns nrOfEntries from a remote cache in map.
     *TODO
     */
    std::map<HR_SHARED_PTR<K>, HR_SHARED_PTR<V> > getBulk(int nrOfEntries) {
      std::map<void*, void*> mbuf;
      base_getBulk(nrOfEntries, &mbuf);

      std::map<HR_SHARED_PTR<K>, HR_SHARED_PTR<V> > result;
        for(std::map<void*, void*>::const_iterator i = mbuf.begin(); i != mbuf.end(); i++) {
            result.insert(std::make_pair(
                HR_SHARED_PTR<K>((K*)i->first), HR_SHARED_PTR<V>((V*)i->second)));
        }
        return result;
    }
    /**
     * Unsupported operation in this release of Hot Rod client. UnsupportedOperationException is
     * thrown if his method is invoked.
     */
    std::set<std::pair<K, V> > entrySet() {
	throw UnsupportedOperationException();
    }
    /**
     * Returns all keys in the remote server.  It'll invoke a command over the network each time this method is called.
     * If the remote cache is a distributed cache, it will retrieve all of the keys from all nodes in the cluster.
     * Please use with care for cache with large data set.
     *
     * \return a std::set of pointers to all keys in this given cache
     */
    std::set<HR_SHARED_PTR<K> > keySet() {
      std::set<void*> p;
      base_keySet(0,&p);

        std::set<HR_SHARED_PTR<K> > result;
        for(std::set<void*>::const_iterator i = p.begin(); i != p.end(); i++) {
        result.insert(HR_SHARED_PTR<K>((K*)*i));
      }
      return result;
    }
    /**
     *Returns an approximate number of key/value pairs in this cache.
     *
     *\return number of entries in cache
     */
    uint64_t size() {
        std::map<std::string,std::string> statistics;
        base_stats(&statistics);
        std::istringstream sizeAsStream;
        sizeAsStream.str(statistics["currentNumberOfEntries"]);
        int result;
        sizeAsStream >> result;
        // TODO: check errors
        return result;
    }
    /**
     *Returns true if and only if this cache has no entries.
     *
     *\return true if cache is empty
     */
    bool isEmpty() {
	return 0 == size();
    }
    /**
     * Unsupported operation in this release of Hot Rod client. UnsupportedOperationException is
     * thrown if his method is invoked.
     */
    std::vector<V> values() {
	throw UnsupportedOperationException();
    }
    /**
     * Returns statistics for this cache.
     *
     * \return a std::map of string key/value pairs where each key is a certain statistic property
     * while the matching value represents the value for that property
     */
    std::map<std::string, std::string> stats() {
        std::map<std::string,std::string> statistics;
        base_stats(&statistics);
        return statistics;
    }
    /**
     * Clears all entries in this cache
     *
     *
     */
    void clear() {
        base_clear();
    }
    /**
     * Pings remote cache on a Hot Rod server
     *
     *
     */
    void ping() {
        base_ping();
    }
    /**
     * Applies one or more Flag enums to the scope of a single invocation.
     *
     * \param flags the flags to apply to an invocation
     * \return the current RemoteCache instance
     */
    RemoteCache<K,V>& withFlags(Flag flags) {
        base_withFlags(flags);
        return *this;
    }

    RemoteCache(const RemoteCache &other) :
      RemoteCacheBase(other), keyMarshaller(other.keyMarshaller), valueMarshaller(other.valueMarshaller)
    {
        setMarshallers(this, &keyMarshall, &valueMarshall, &keyUnmarshall, &valueUnmarshall);
    }

    RemoteCache<K,V>& operator=(const RemoteCache& other) {
        RemoteCacheBase::operator=(other);
        keyMarshaller = other.keyMarshaller;
        valueMarshaller = other.valueMarshaller;
        setMarshallers(this, &keyMarshall, &valueMarshall, &keyUnmarshall, &valueUnmarshall);
        return *this;
    }

  private:
    RemoteCache() : RemoteCacheBase() {
        setMarshallers(this, &keyMarshall, &valueMarshall, &keyUnmarshall, &valueUnmarshall);
    }

    uint64_t toSeconds(uint64_t time, TimeUnit unit) {
	uint64_t result;
	switch (unit) {
	case NANOSECONDS:
	    result = ceil(time / 1000000000.0);
	    break;
	case MICROSECONDS:
	    result = ceil(time / 1000000.0);
	    break;
	case MILLISECONDS:
	    result = ceil(time / 1000.0);
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
    static void keyMarshall(void *thisp, const void* key, void* buf) {
        ((RemoteCache<K, V> *) thisp)->keyMarshaller->marshall(*(const K *) key, *(ScopedBuffer *) buf);
    }
    static void valueMarshall(void* thisp, const void* val, void* buf) {
        ((RemoteCache<K, V> *)thisp)->valueMarshaller->marshall(*(const V *) val, *(ScopedBuffer *) buf);
    }
    static void* keyUnmarshall(void *thisp, const void* buf) {
        return ((RemoteCache<K, V> *) thisp)->keyMarshaller->unmarshall(*(const ScopedBuffer *) buf);
    }
    static void* valueUnmarshall(void* thisp, const void* buf) {
        return ((RemoteCache<K, V> *)thisp)->valueMarshaller->unmarshall(*(const ScopedBuffer *) buf);
    }

    HR_SHARED_PTR<Marshaller<K> > keyMarshaller;
    HR_SHARED_PTR<Marshaller<V> > valueMarshaller;

  friend class RemoteCacheManager;
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHE_H */
