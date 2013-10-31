/* test jni api version */

package org.infinispan.client.hotrod;

import java.util.Map;

import org.infinispan.client.hotrod.RemoteCacheManager;
import org.infinispan.client.hotrod.MetadataValue;
import org.infinispan.client.hotrod.VersionedValue;

public interface RemoteCache<K, V> {

    V put(K k, V v);

    V get(K k);

    V remove(K k);

    void clear();

    boolean containsKey(K k);

    V replace(K k, V v);

    //V replaceWithVersion(K k, V nv, long version);

    V putIfAbsent(K k, V v);

    VersionedValue<V> getVersioned(K k);

    MetadataValue<V> getWithMetadata(K key);

    Map<K, V> getBulk(int size);
}
