/* test jni api version */

package org.infinispan.client.hotrod;

// originals
import org.infinispan.marshall.Marshaller;
import java.util.Properties;

// jni wrappers
import org.infinispan.client.hotrod.RemoteCache;
import org.infinispan.client.hotrod.impl.RemoteCacheImpl;
import org.infinispan.client.hotrod.jni.HotrodConstants;
import org.infinispan.client.hotrod.jni.MapType;

public class RemoteCacheManager /* implements BasicCacheContainer */ {
	
	private static final String ISPN_CLIENT_HOTROD_SERVER_LIST = "infinispan.client.hotrod.server_list";

    private org.infinispan.client.hotrod.jni.RemoteCacheManager jniRemoteCacheManager;
    private org.infinispan.client.hotrod.jni.Configuration jniConfiguration;
    private org.infinispan.client.hotrod.jni.ConfigurationBuilder jniConfigurationBuilder;
    private Marshaller marshaller;

    public RemoteCacheManager() {
        jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager();
	marshaller = new org.infinispan.marshall.jboss.GenericJBossMarshaller();
    }
    
    public RemoteCacheManager(String servers, boolean start) {
    	MapType converter = new MapType();
    	converter.set(ISPN_CLIENT_HOTROD_SERVER_LIST, servers);
    	jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(converter, true);
    	marshaller = new org.infinispan.marshall.jboss.GenericJBossMarshaller();
    }
    
    public RemoteCacheManager(Properties props) {
    	
    	MapType converter = new MapType();
    	converter.set(ISPN_CLIENT_HOTROD_SERVER_LIST, (String)props.get(ISPN_CLIENT_HOTROD_SERVER_LIST));
    	jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(converter, true);
    	marshaller = new org.infinispan.marshall.jboss.GenericJBossMarshaller();
    }
    
    

    public <K, V> org.infinispan.client.hotrod.RemoteCache <K, V> getCache() {
        return new org.infinispan.client.hotrod.impl.RemoteCacheImpl<K, V>(this, "");
    }

    public <K, V> org.infinispan.client.hotrod.RemoteCache <K, V> getNativeCache() {
        return new org.infinispan.client.hotrod.impl.RemoteCacheImpl<K, V>(this, "native");
    }
    
    public <K, V> org.infinispan.client.hotrod.RemoteCache <K, V> getCache(String cacheName) {
    	return new org.infinispan.client.hotrod.impl.RemoteCacheImpl<K, V>(this, cacheName);
    }

    public Marshaller getMarshaller() {
    	return marshaller;
    }

    public org.infinispan.client.hotrod.jni.RemoteCacheManager getJniManager() {
    	return jniRemoteCacheManager;
    }

    static {
	System.loadLibrary("hotrod-jni");
    }
}
