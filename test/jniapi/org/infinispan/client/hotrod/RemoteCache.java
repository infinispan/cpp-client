/* test jni api version */

package org.infinispan.client.hotrod;

import org.infinispan.client.hotrod.RemoteCacheManager;

public interface RemoteCache<K, V> {

    public V put(K k, V v);

    public V get(K k);
}
