package org.infinispan.client.hotrod.impl;

import java.util.Collection;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.TimeUnit;

import org.infinispan.client.hotrod.RemoteCache;
import org.infinispan.commons.util.concurrent.NotifyingFuture;

public abstract class RemoteCacheUnsupported<K, V> implements RemoteCache<K, V> {

    @Override
    public boolean remove(K key, V value) {
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
    public boolean replace(K k, V v1, V v2, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle, TimeUnit maxIdleUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean replace(K k, V v1, V v2) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<Void> clearAsync() {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<V> getAsync(K k) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<Void> putAllAsync(Map<? extends K, ? extends V> map) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<Void> putAllAsync(Map<? extends K, ? extends V> map, long arg1, TimeUnit arg2) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<Void> putAllAsync(Map<? extends K, ? extends V> map, long lifespan,
            TimeUnit lifespanTimeUnit, long maxIdle, TimeUnit maxIdleUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<V> putAsync(K k, V v) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<V> putAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<V> putAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
            TimeUnit maxIdleUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<V> putIfAbsentAsync(K k, V v) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<V> putIfAbsentAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<V> putIfAbsentAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
            TimeUnit maxIdleUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<V> removeAsync(K k) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<Boolean> removeAsync(K k, V v) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<V> replaceAsync(K k, V v) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<Boolean> replaceAsync(K k, V v1, V v2) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<V> replaceAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<Boolean> replaceAsync(K k, V v1, V v2, long lifespan, TimeUnit lifespanTimeUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<V> replaceAsync(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle,
            TimeUnit maxIdleUnit) {
        throw new UnsupportedOperationException();
    }

    @Override
    public NotifyingFuture<Boolean> replaceAsync(K k, V v1, V v2, long lifespan, TimeUnit lifespanTimeUnit,
            long maxIdle, TimeUnit maxIdleUnit) {
        throw new UnsupportedOperationException();
    }

}
