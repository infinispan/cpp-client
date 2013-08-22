/* test jni api version */

package org.infinispan.client.hotrod;

import org.infinispan.client.hotrod.RemoteCacheManager;

public interface RemoteCache<K, V> {

    public V put(K k, V v);

    public V get(K k);
    
    public V remove(K k);
    
    public void clear();
    
    public boolean containsKey(K k);
    
    public V replace(K k, V v);
    
    //public V replaceWithVersion(K k, V nv, long version);
    
    public V putIfAbsent(K k, V v);
    
    //public VersionedValue<V> getVersioned(K k);
    
}
