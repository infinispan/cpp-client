package org.infinispan.client.hotrod.impl;

import java.util.Collection;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.TimeUnit;

import org.infinispan.client.hotrod.RemoteCache;

public abstract class RemoteCacheUnsupported<K, V> implements RemoteCache<K, V> {

    @Override
    public <T> T execute(String scriptName, Map<String, ?> params) {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean remove(Object key, Object value) {
        throw new UnsupportedOperationException();
    }

    @Override
    public Collection<V> values() {
        throw new UnsupportedOperationException();
    }

    @Override
    public Set<java.util.Map.Entry<K, V>> entrySet() {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean replace(K k, V v1, V v2, long lifespan, TimeUnit lifespanTimeUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
   public boolean replace(K k, V v1, V v2, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
         TimeUnit maxIdleUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean replace(K k, V v1, V v2) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<Void> clearAsync() {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<V> getAsync(K k) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<Void> putAllAsync(Map<? extends K, ? extends V> map) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<Void> putAllAsync(Map<? extends K, ? extends V> map, long arg1, TimeUnit arg2) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<Void> putAllAsync(Map<? extends K, ? extends V> map, long lifespan,
            TimeUnit lifespanTimeUnit, long maxIdle, TimeUnit maxIdleUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<V> putAsync(K k, V v) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<V> putAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<V> putAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
            TimeUnit maxIdleUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<V> putIfAbsentAsync(K k, V v) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<V> putIfAbsentAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<V> putIfAbsentAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
            TimeUnit maxIdleUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<V> removeAsync(Object k) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<Boolean> removeAsync(Object k, Object v) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<V> replaceAsync(K k, V v) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<Boolean> replaceAsync(K k, V v1, V v2) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<V> replaceAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<Boolean> replaceAsync(K k, V v1, V v2, long lifespan, TimeUnit lifespanTimeUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<V> replaceAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
            TimeUnit maxIdleUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<Boolean> replaceAsync(K k, V v1, V v2, long lifespan, TimeUnit lifespanTimeUnit,
            long maxIdle, TimeUnit maxIdleUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<Boolean> replaceWithVersionAsync(K key, V newValue, long version) {
       throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<Boolean> replaceWithVersionAsync(K key, V newValue, long version, int lifespanSeconds) {
       throw new UnsupportedOperationException();
    }

    @Override
   public CompletableFuture<Boolean> replaceWithVersionAsync(K key, V newValue, long version, int lifespanSeconds,
         int maxIdleSeconds) {
       throw new UnsupportedOperationException();
    }
    
    @Override
   public CompletableFuture<Boolean> removeWithVersionAsync(K key, long version) {
       throw new UnsupportedOperationException();
    }

}
