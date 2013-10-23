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

template <class K, class V> class RemoteCache : private RemoteCacheBase
{
  public:
    std::string getName() {
	throw UnsupportedOperationException();
    }

    std::string getVersion() {
	throw UnsupportedOperationException();
    }

    std::string getProtocolVersion() {
	throw UnsupportedOperationException();
    }

    V* get(const K& key) {
        ScopedBuffer vbuf;
        base_get(&key, &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }

    V* put(
        const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
	return put(key, val, lifespan, SECONDS, maxIdle, SECONDS);
    }

    V* put(
        const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit)
    {
	return put(key, val, lifespan, lifespanUnit, 0, SECONDS);
    }

    V* put(
	const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle, TimeUnit maxIdleUnit)
    {
	ScopedBuffer vbuf;
        base_put(&key, &val, toSeconds(lifespan, lifespanUnit), toSeconds(maxIdle, maxIdleUnit), &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }

    V* putIfAbsent(
        const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
	return putIfAbsent(key, val, lifespan, SECONDS, maxIdle, SECONDS);
    }

    V* putIfAbsent(
		   const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit)
    {
	return putIfAbsent(key, val, lifespan, lifespanUnit, 0, SECONDS);
    }

    V* putIfAbsent(
		   const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle, TimeUnit maxIdleUnit)
    {
	ScopedBuffer vbuf;
        base_putIfAbsent(&key, &val, toSeconds(lifespan, lifespanUnit), toSeconds(maxIdle, maxIdleUnit), &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }

    void putAll(const std::map<K, V>& map, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
	return putAll(map, lifespan, SECONDS, maxIdle, SECONDS);
    }

    void putAll(const std::map<K, V>& map, uint64_t lifespan, TimeUnit lifespanUnit)
    {
	return putAll(map, lifespan, lifespanUnit, 0, SECONDS);
    }

    void putAll(const std::map<K, V>& map, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle, TimeUnit maxIdleUnit)
    {
	uint64_t lifespanMillis = toSeconds(lifespan, lifespanUnit);
	uint64_t maxIdleMillis = toSeconds(maxIdle, maxIdleUnit);

	for (typename std::map<K, V>::const_iterator it = map.begin(); it != map.end(); ++it) {
	    put(it->first, it->second, lifespanMillis, maxIdleMillis);
	}
    }

    V* replace(
	       const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
	return replace(key, val, lifespan, SECONDS, maxIdle, SECONDS);
    }

    V* replace(
	       const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit)
    {
	return replace(key, val, lifespan, lifespanUnit, 0, SECONDS);
    }

    V* replace(
	       const K& key, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle, TimeUnit maxIdleUnit)
    {
        ScopedBuffer vbuf;
        base_replace(&key, &val, toSeconds(lifespan, lifespanUnit), toSeconds(maxIdle, maxIdleUnit), &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }

    V* replace(
	       const K& key, const V& oldVal, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
	return replace(key, oldVal, val, lifespan, SECONDS, maxIdle, SECONDS);
    }

    V* replace(
	       const K& key, const V& oldVal, const V& val, uint64_t lifespan, TimeUnit lifespanUnit)
    {
	return replace(key, oldVal, val, lifespan, lifespanUnit, 0, SECONDS);
    }

    V* replace(
	       const K& key, const V& oldVal, const V& val, uint64_t lifespan, TimeUnit lifespanUnit, uint64_t maxIdle, TimeUnit maxIdleUnit)
    {
	throw UnsupportedOperationException();
    }


    V* remove(const K& key) {
        ScopedBuffer vbuf;
        base_remove(&key, &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }

    bool containsKey(const K& key) {
        bool res;
        base_containsKey(&key, &res);
        return res;
    }

    bool containsValue(const V& val) {
	throw UnsupportedOperationException();
    }

    bool replaceWithVersion(
        const K& key, const V& val,
        uint64_t version, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
        bool res;
        base_replaceWithVersion(&key, &val, version, lifespan, maxIdle, &res);
        return res;
    }

    bool removeWithVersion(const K& key, uint64_t version) {
        bool res;
        base_removeWithVersion(&key, version, &res);
        return res;
    }

    std::pair<HR_SHARED_PTR<V>, VersionedValue> getVersioned(const K& key) {
	throw UnsupportedOperationException();
    }

    std::pair<HR_SHARED_PTR<V>, MetadataValue> getWithMetadata(const K& key) {
        ScopedBuffer vbuf;
        MetadataValue metadata;
        base_getWithMetadata(&key, &vbuf, &metadata);
        return vbuf.getBytes() ?
            std::make_pair(HR_SHARED_PTR<V>(valueMarshaller->unmarshall(vbuf)), metadata) :
            std::make_pair(HR_SHARED_PTR<V>(), metadata);
    }

    std::map<HR_SHARED_PTR<K>, HR_SHARED_PTR<V> > getBulk() {
	throw UnsupportedOperationException();
    }

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

    std::set<std::pair<K, V> > entrySet() {
	throw UnsupportedOperationException();
    }

    std::set<HR_SHARED_PTR<K> > keySet() {
      std::set<void*> p;
      base_keySet(0,&p);

        std::set<HR_SHARED_PTR<K> > result;
        for(std::set<void*>::const_iterator i = p.begin(); i != p.end(); i++) {
        result.insert(HR_SHARED_PTR<K>((K*)*i));
      }
      return result;
    }

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

    bool isEmpty() {
	return 0 == size();
    }

    std::vector<V> values() {
	throw UnsupportedOperationException();
    }

    std::map<std::string, std::string> stats() {
        std::map<std::string,std::string> statistics;
        base_stats(&statistics);
        return statistics;
    }

    void clear() {
        base_clear();
    }

    void ping() {
        base_ping();
    }

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
