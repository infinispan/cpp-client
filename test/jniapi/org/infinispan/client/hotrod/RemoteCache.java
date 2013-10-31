/* test jni api version */

package org.infinispan.client.hotrod;

import java.util.Map;
import java.util.concurrent.TimeUnit;

import org.infinispan.client.hotrod.MetadataValue;
import org.infinispan.client.hotrod.VersionedValue;

public interface RemoteCache<K, V> {

    V put(K k, V v);

    V put(K key, V value, long lifespan, TimeUnit unit);

    V put(K key, V value, long lifespan, TimeUnit lifespanUnit, long maxIdleTime, TimeUnit maxIdleTimeUnit);

    V putIfAbsent(K k, V v);

    V putIfAbsent(K key, V value, long lifespan, TimeUnit unit);

    V putIfAbsent(K key, V value, long lifespan, TimeUnit lifespanUnit, long maxIdleTime, TimeUnit maxIdleTimeUnit);

    V get(K k);

    V remove(K k);

    void clear();

    boolean containsKey(K k);
    
    boolean containsValue(V v);

    V replace(K k, V v);

    V replace(K key, V value, long lifespan, TimeUnit unit);

    V replace(K key, V value, long lifespan, TimeUnit lifespanUnit, long maxIdleTime, TimeUnit maxIdleTimeUnit);

    //V replaceWithVersion(K k, V nv, long version);

    VersionedValue<V> getVersioned(K k);

    MetadataValue<V> getWithMetadata(K key);

    Map<K, V> getBulk(int size);
    
    boolean isEmpty();
}
