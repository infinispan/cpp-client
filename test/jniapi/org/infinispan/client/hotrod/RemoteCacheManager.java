/* test jni api version */

package org.infinispan.client.hotrod;

// originals
import org.infinispan.marshall.Marshaller;

// jni wrappers
import org.infinispan.client.hotrod.RemoteCache;
import org.infinispan.client.hotrod.RemoteCacheImpl;
import org.infinispan.client.hotrod.jni.HotrodConstants;

public class RemoteCacheManager /* implements BasicCacheContainer */ {

    private org.infinispan.client.hotrod.jni.RemoteCacheManager jniRemoteCacheManager;
    private Marshaller marshaller;

    public RemoteCacheManager() {
        jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager();
	marshaller = new org.infinispan.marshall.jboss.GenericJBossMarshaller();
    }

    public <K, V> org.infinispan.client.hotrod.RemoteCache <K, V> getCache() {
        return new org.infinispan.client.hotrod.RemoteCacheImpl<K, V>(this, "");
    }

    public <K, V> org.infinispan.client.hotrod.RemoteCache <K, V> getNativeCache() {
        return new org.infinispan.client.hotrod.RemoteCacheImpl<K, V>(this, "native");
    }

    public Marshaller getMarshaller() {
	return marshaller;
    }

    protected org.infinispan.client.hotrod.jni.RemoteCacheManager getJniManager() {
	return jniRemoteCacheManager;
    }

    static {
	System.loadLibrary("hotrod-jni");
    }
}
