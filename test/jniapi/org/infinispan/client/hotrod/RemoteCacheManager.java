/* test jni api version */

package org.infinispan.client.hotrod;

// originals
import java.util.Properties;

import org.infinispan.client.hotrod.jni.MapType;
import org.infinispan.commons.marshall.Marshaller;
// jni wrappers

public class RemoteCacheManager /* implements BasicCacheContainer */{

    private static final String ISPN_CLIENT_HOTROD_SERVER_LIST = "infinispan.client.hotrod.server_list";

    private org.infinispan.client.hotrod.jni.RemoteCacheManager jniRemoteCacheManager;
    private org.infinispan.client.hotrod.jni.Configuration jniConfiguration;
    private org.infinispan.client.hotrod.jni.ConfigurationBuilder jniConfigurationBuilder;
    private Marshaller marshaller;

    public RemoteCacheManager() {
        jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager();
        marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
    }

    public RemoteCacheManager(String servers) {
        this(servers, true);
    }

    public RemoteCacheManager(String server, int port) {
        this(String.format("%s:%d", server, port), true);
    }

    public RemoteCacheManager(String servers, boolean start) {
        MapType converter = new MapType();
        converter.set(ISPN_CLIENT_HOTROD_SERVER_LIST, servers);
        jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(converter, true);
        marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
    }

    public RemoteCacheManager(Properties props) {

        MapType converter = new MapType();
        converter.set(ISPN_CLIENT_HOTROD_SERVER_LIST, (String) props.get(ISPN_CLIENT_HOTROD_SERVER_LIST));
        jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(converter, true);
        marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
    }

    public <K, V> org.infinispan.client.hotrod.RemoteCache<K, V> getCache() {
        return new org.infinispan.client.hotrod.impl.RemoteCacheImpl<K, V>(this, "", false);
    }

    public <K, V> org.infinispan.client.hotrod.RemoteCache<K, V> getCache(boolean forceReturnValue) {
        return new org.infinispan.client.hotrod.impl.RemoteCacheImpl<K, V>(this, "", forceReturnValue);
    }

    public <K, V> org.infinispan.client.hotrod.RemoteCache<K, V> getCache(String cacheName) {
        return new org.infinispan.client.hotrod.impl.RemoteCacheImpl<K, V>(this, cacheName, false);
    }

    public <K, V> org.infinispan.client.hotrod.RemoteCache<K, V> getCache(String cacheName, boolean forceReturnValue) {
        return new org.infinispan.client.hotrod.impl.RemoteCacheImpl<K, V>(this, cacheName, forceReturnValue);
    }

    public Marshaller getMarshaller() {
        return marshaller;
    }

    public org.infinispan.client.hotrod.jni.RemoteCacheManager getJniManager() {
        return jniRemoteCacheManager;
    }

    static {
        System.loadLibrary("hotrod");
        System.loadLibrary("hotrod-jni");
    }
}
