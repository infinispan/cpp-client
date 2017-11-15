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
    private Configuration configuration;
    private boolean forceReturnValue;

    public RemoteCacheManager() {
        jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager();
        marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
        configuration = null;
        forceReturnValue = false;
    }

    public RemoteCacheManager(boolean start) {
        jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(start);
        marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
        configuration = null;
        forceReturnValue = false;
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
        jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(converter, start);
        marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
        configuration = null;
        forceReturnValue = false;
    }

    public RemoteCacheManager(Configuration config) {
       this(config, true);
       configuration = config;
       forceReturnValue = config.forceReturnValues();
    }

    public RemoteCacheManager(Configuration config, boolean start) {
       jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(config.getJniConfiguration(), start);
       marshaller = (config.marshaller()!=null) ? config.marshaller() :
           new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
       configuration = config;
       forceReturnValue = config.forceReturnValues();
    }

   public RemoteCacheManager(URL config, boolean start) throws IOException {
      Properties props = new Properties();
      props.load(config.openStream());
      org.infinispan.client.hotrod.jni.Configuration jniConfiguration = new ConfigurationBuilder()
            .withProperties(props).build().getJniConfiguration();
      jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(jniConfiguration, start);
      marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
      configuration = null;
      forceReturnValue = jniConfiguration.isForceReturnValue();
   }
    
    public RemoteCacheManager(Properties props) {
       this(props, true);
    }
    
    public RemoteCacheManager(Properties props, boolean start) {
      Configuration config = new ConfigurationBuilder().withProperties(props).build();
      jniRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(config.getJniConfiguration(), start);
      marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();
      configuration = null;
      forceReturnValue = config.getJniConfiguration().isForceReturnValue();
    }
    
    public <K, V> org.infinispan.client.hotrod.RemoteCache<K, V> getCache() {
       try {
          return new org.infinispan.client.hotrod.impl.RemoteCacheImpl<K, V>(this, "", this.forceReturnValue);
       } catch (Exception e) {
          return null;
       }
    }

    public <K, V> org.infinispan.client.hotrod.RemoteCache<K, V> getCache(boolean forceReturnValue) {
       try {
          return new org.infinispan.client.hotrod.impl.RemoteCacheImpl<K, V>(this, "", forceReturnValue);
       } catch (Exception e) {
          return null;
       }
    }

    public <K, V> org.infinispan.client.hotrod.RemoteCache<K, V> getCache(String cacheName) {
       try {
          return new org.infinispan.client.hotrod.impl.RemoteCacheImpl<K, V>(this, cacheName, this.forceReturnValue);
       } catch (Exception e) {
          return null;
       }
    }

    public <K, V> org.infinispan.client.hotrod.RemoteCache<K, V> getCache(String cacheName, boolean forceReturnValue) {
       try {
          return new org.infinispan.client.hotrod.impl.RemoteCacheImpl<K, V>(this, cacheName, forceReturnValue);
       } catch (Exception e) {
          return null;
       }
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

    public boolean switchToDefaultCluster() {
       return jniRemoteCacheManager.switchToDefaultCluster();
    }

    public boolean switchToCluster(String clusterName) {
       return jniRemoteCacheManager.switchToCluster(clusterName);
    }

    static {
        try {
            System.loadLibrary("hotrod");
        } catch (UnsatisfiedLinkError e) {
            System.loadLibrary("hotrod32");
        }
        System.loadLibrary("hotrod-jni");
    }
}
