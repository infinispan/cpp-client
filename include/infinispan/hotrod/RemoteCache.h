#ifndef ISPN_HOTROD_REMOTECACHE_H
#define ISPN_HOTROD_REMOTECACHE_H



#include "infinispan/hotrod/RemoteCacheBase.h"
#include "infinispan/hotrod/Marshaller.h"
#include "infinispan/hotrod/Flag.h"
#include "infinispan/hotrod/MetadataValue.h"

#include <set>
#include <map>
#include <sstream>

namespace infinispan {
namespace hotrod {

template <class K, class V> class RemoteCache : private RemoteCacheBase
{
  public:
    V* get(const K& key) {
        ScopedBuffer vbuf;
        base_get(&key, &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }

    V* put(
        const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
        ScopedBuffer vbuf;
        base_put(&key, &val, lifespan, maxIdle, &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }

    V* putIfAbsent(
        const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
        ScopedBuffer vbuf;
        base_putIfAbsent(&key, &val, lifespan, maxIdle, &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }

    V* replace(
        const K& key, const V& val, uint64_t lifespan = 0, uint64_t maxIdle = 0)
    {
        ScopedBuffer vbuf;
        base_replace(&key, &val, lifespan, maxIdle, &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
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

    std::pair<HR_SHARED_PTR<V>, MetadataValue> getWithMetadata(const K& key) {
        ScopedBuffer vbuf;
        MetadataValue metadata;
        base_getWithMetadata(&key, &vbuf, &metadata);
        return vbuf.getBytes() ?
            std::make_pair(HR_SHARED_PTR<V>(valueMarshaller->unmarshall(vbuf)), metadata) :
            std::make_pair(HR_SHARED_PTR<V>(), metadata);
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

  private:
    RemoteCache(const std::string& name) : RemoteCacheBase(name) {
        setMarshallers(this, &keyMarshall, &valueMarshall, &keyUnmarshall, &valueUnmarshall);
    }
    // type-hiding and resurrecting support
    static void keyMarshall(void *thisp, const void* key, void* buf) {
        ((RemoteCache<K, V> *) thisp)->keyMarshaller->marshall(*(K *) key, *(ScopedBuffer *) buf);
    }
    static void valueMarshall(void* thisp, const void* val, void* buf) {
        ((RemoteCache<K, V> *)thisp)->valueMarshaller->marshall(*(V *) val, *(ScopedBuffer *) buf);
    }
    static void* keyUnmarshall(void *thisp, const void* buf) {
        return ((RemoteCache<K, V> *) thisp)->keyMarshaller->unmarshall(*(ScopedBuffer *) buf);
    }
    static void* valueUnmarshall(void* thisp, const void* buf) {
        return ((RemoteCache<K, V> *)thisp)->valueMarshaller->unmarshall(*(ScopedBuffer *) buf);
    }

    HR_SHARED_PTR<Marshaller<K> > keyMarshaller;
    HR_SHARED_PTR<Marshaller<V> > valueMarshaller;

  friend class RemoteCacheManager;
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHE_H */
