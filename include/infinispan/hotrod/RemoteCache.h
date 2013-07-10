#ifndef ISPN_HOTROD_REMOTECACHE_H
#define ISPN_HOTROD_REMOTECACHE_H

#include "infinispan/hotrod/RemoteCacheBase.h"
#include "infinispan/hotrod/Marshaller.h"

namespace infinispan {
namespace hotrod {

template <class K, class V>
class RemoteCache : private RemoteCacheBase
{
  public:
    V* get(const K& k) {
        ScopedBuffer vbuf;
        base_get(&k, &vbuf);
        return vbuf.getBytes() ? valueMarshaller->unmarshall(vbuf) : NULL;
    }
    void put(const K& k, const V& v) {
        base_put(&k, &v);;
    }
    // Need separate signature for ForceReturnPreviousValue:   V put(const K& k, const V& v)
    void ping() {
        base_ping();
    }

  private:
    RemoteCache() {
        setMarshallers(this, &keyMarshall, &valueMarshall);
    }
    // type-hiding and resurrecting support
    static void keyMarshall(void *thisp, const void* key, void* buf) {
        ((RemoteCache<K, V> *) thisp)->keyMarshaller->marshall(*(K *) key, *(ScopedBuffer *) buf);
    }
    static void valueMarshall(void* thisp, const void* val, void* buf) {
        ((RemoteCache<K, V> *)thisp)->valueMarshaller->marshall(*(V *) val, *(ScopedBuffer *) buf);
    }

    HR_SHARED_PTR<Marshaller<K> > keyMarshaller;
    HR_SHARED_PTR<Marshaller<V> > valueMarshaller;

  friend class RemoteCacheManager;
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHE_H */
