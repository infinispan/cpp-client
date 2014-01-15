/* test jni api version */

package org.infinispan.client.hotrod;

// originals
import java.io.IOException;
import java.net.URL;
import java.util.Properties;

import org.infinispan.client.hotrod.configuration.Configuration;
import org.infinispan.client.hotrod.configuration.ConfigurationBuilder;
import org.infinispan.client.hotrod.jni.MapType;
import org.infinispan.commons.marshall.Marshaller;
// jni wrappers

public class RemoteCacheManager /* implements BasicCacheContainer */{

    private static final String ISPN_CLIENT_HOTROD_SERVER_LIST = "infinispan.client.hotrod.server_list";

    private org.infinispan.client.hotrod.jni.RemoteCacheManager jniRemoteCacheManager;
    private Marshaller marshaller;

    public RemoteCacheManager() {
        jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager();
        marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
    }

    public RemoteCacheManager(boolean start) {
        jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(start);
        marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
    }

    public RemoteCacheManager(String servers) {
        this(servers, true);
    }

    public RemoteCacheManager(String server, int port) {
        this(String.format("%s:%d", server, port), true);
    }

    public RemoteCacheManager(String server, int port, boolean start) {
        this(String.format("%s:%d", server, port), start);
    }

    public RemoteCacheManager(String servers, boolean start) {
        MapType converter = new MapType();
        converter.set(ISPN_CLIENT_HOTROD_SERVER_LIST, servers);
        jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(converter, true);
        marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
    }

    public RemoteCacheManager(Configuration config) {
       this(config, true);
    }

    public RemoteCacheManager(Configuration config, boolean start) {
       jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(config.getJniConfiguration(), start);
       marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
    }

   public RemoteCacheManager(URL config, boolean start) throws IOException {
      Properties props = new Properties();
      props.load(config.openStream());
      jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(new ConfigurationBuilder()
            .withProperties(props).build().getJniConfiguration(), start);
      marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
   }
    
    public RemoteCacheManager(Properties props) {
       this(props, true);
    }
    
    public RemoteCacheManager(Properties props, boolean start) {
      jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(new ConfigurationBuilder()
            .withProperties(props).build().getJniConfiguration(), start);
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
    
    public boolean isStarted() {
       return jniRemoteCacheManager.isStarted();
    }
    
    public void start() {
       jniRemoteCacheManager.start();
    }
    
    public void stop() {
       jniRemoteCacheManager.stop();
    }
    
    public Properties getProperties() {
       throw new UnsupportedOperationException();
    }

    static {
        System.loadLibrary("hotrod");
        System.loadLibrary("hotrod-jni");
    }
}
