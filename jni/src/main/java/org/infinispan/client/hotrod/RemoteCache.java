/* test jni api version */

package org.infinispan.client.hotrod;

import java.util.Collection;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.TimeUnit;

import org.infinispan.client.hotrod.CacheTopologyInfo;

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
    
    Map<K, V> getAll(Set<K> keySet);

    boolean isEmpty();

    RemoteCache<K, V> withFlags(Flag... flags);

    int size();

    boolean replace(K k, V v1, V v2);

   CompletableFuture<Boolean> replaceAsync(K k, V v1, V v2, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
         TimeUnit maxIdleTimeUnit);

   CompletableFuture<V> replaceAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
            TimeUnit maxIdleTimeUnit);

   CompletableFuture<Boolean> replaceAsync(K k, V v1, V v2, long lifespan, TimeUnit lifespanTimeUnit);

   CompletableFuture<V> replaceAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit);

   CompletableFuture<Boolean> replaceAsync(K k, V v1, V v2);

   CompletableFuture<V> replaceAsync(K k, V v);

   CompletableFuture<Boolean> removeAsync(K k, V v);

   CompletableFuture<V> removeAsync(K k);

   CompletableFuture<V> putIfAbsentAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
            TimeUnit maxIdleTimeUnit);

   CompletableFuture<V> putIfAbsentAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit);

   CompletableFuture<V> putIfAbsentAsync(K k, V v);

   CompletableFuture<V> putAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
            TimeUnit maxIdleTimeUnit);

   CompletableFuture<V> putAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit);

   CompletableFuture<V> putAsync(K k, V v);

   CompletableFuture<Void> putAllAsync(Map<? extends K, ? extends V> map, long lifespan, TimeUnit lifespanTimeUnit,
            long maxIdle, TimeUnit maxIdleUnit);

   CompletableFuture<Void> putAllAsync(Map<? extends K, ? extends V> map, long arg1, TimeUnit arg2);

   CompletableFuture<Void> putAllAsync(Map<? extends K, ? extends V> map);

   CompletableFuture<V> getAsync(K k);

   CompletableFuture<Void> clearAsync();

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

   CompletableFuture<Boolean> replaceWithVersionAsync(K key, V newValue, long version);
    
   CompletableFuture<Boolean> replaceWithVersionAsync(K key, V newValue, long version, int lifespanSeconds);
    
   CompletableFuture<Boolean> replaceWithVersionAsync(K key, V newValue, long version, int lifespanSeconds,
         int maxIdleSeconds);

   CompletableFuture<Boolean> removeWithVersionAsync(K key, long version);
    CacheTopologyInfo getCacheTopologyInfo();
}
