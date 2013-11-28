/* test jni api version */

package org.infinispan.client.hotrod;

import java.util.Collection;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.concurrent.TimeUnit;

import org.infinispan.commons.util.concurrent.NotifyingFuture;

public interface RemoteCache<K, V> {
    String getName();

    String getVersion();

    String getProtocolVersion();

    void start();

    void stop();

    V put(K k, V v);

    V put(K key, V value, long lifespan, TimeUnit unit);

    V put(K key, V value, long lifespan, TimeUnit lifespanUnit, long maxIdleTime, TimeUnit maxIdleTimeUnit);

    V putIfAbsent(K k, V v);

    V putIfAbsent(K key, V value, long lifespan, TimeUnit unit);

    V putIfAbsent(K key, V value, long lifespan, TimeUnit lifespanUnit, long maxIdleTime, TimeUnit maxIdleTimeUnit);

    V get(K k);

    V remove(K k);

    boolean removeWithVersion(K key, long version);

    void clear();

    boolean containsKey(K k);

    boolean containsValue(V v);

    V replace(K k, V v);

    V replace(K key, V value, long lifespan, TimeUnit unit);

    V replace(K key, V value, long lifespan, TimeUnit lifespanUnit, long maxIdleTime, TimeUnit maxIdleTimeUnit);

    boolean replaceWithVersion(K key, V newValue, long version);

    boolean replaceWithVersion(K key, V newValue, long version, int lifespanSeconds);

    boolean replaceWithVersion(K key, V newValue, long version, int lifespanSeconds, int maxIdleTimeSeconds);

    VersionedValue<V> getVersioned(K k);

    MetadataValue<V> getWithMetadata(K key);

    Map<K, V> getBulk();

    Map<K, V> getBulk(int size);

    boolean isEmpty();

    RemoteCache<K, V> withFlags(Flag... flags);

    int size();

    boolean replace(K k, V v1, V v2);

    NotifyingFuture<Boolean> replaceAsync(K k, V v1, V v2, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle, TimeUnit maxIdleTimeUnit);

    NotifyingFuture<V> replaceAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
            TimeUnit maxIdleTimeUnit);

    NotifyingFuture<Boolean> replaceAsync(K k, V v1, V v2, long lifespan, TimeUnit lifespanTimeUnit);

    NotifyingFuture<V> replaceAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit);

    NotifyingFuture<Boolean> replaceAsync(K k, V v1, V v2);

    NotifyingFuture<V> replaceAsync(K k, V v);

    NotifyingFuture<Boolean> removeAsync(K k, V v);

    NotifyingFuture<V> removeAsync(K k);

    NotifyingFuture<V> putIfAbsentAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
            TimeUnit maxIdleTimeUnit);

    NotifyingFuture<V> putIfAbsentAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit);

    NotifyingFuture<V> putIfAbsentAsync(K k, V v);

    NotifyingFuture<V> putAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
            TimeUnit maxIdleTimeUnit);

    NotifyingFuture<V> putAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit);

    NotifyingFuture<V> putAsync(K k, V v);

    NotifyingFuture<Void> putAllAsync(Map<? extends K, ? extends V> map, long lifespan, TimeUnit lifespanTimeUnit,
            long maxIdle, TimeUnit maxIdleUnit);

    NotifyingFuture<Void> putAllAsync(Map<? extends K, ? extends V> map, long arg1, TimeUnit arg2);

    NotifyingFuture<Void> putAllAsync(Map<? extends K, ? extends V> map);

    NotifyingFuture<V> getAsync(K k);

    NotifyingFuture<Void> clearAsync();

    boolean replace(K k, V v1, V v2, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle, TimeUnit maxIdleUnit);

    boolean replace(K k, V v1, V v2, long lifespan, TimeUnit lifespanTimeUnit);

    void putAll(Map<? extends K, ? extends V> map, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
            TimeUnit maxIdleUnit);

    void putAll(Map<? extends K, ? extends V> map, long lifespan, TimeUnit lifespanTimeUnit);

    void putAll(Map<? extends K, ? extends V> m);

    Set<K> keySet();

    Collection<V> values();

    Set<Entry<K, V>> entrySet();

    boolean remove(K key, V value);

    ServerStatistics stats();
}
