package org.infinispan.client.hotrod;

import java.util.Arrays;
import java.util.EnumSet;

import org.infinispan.client.hotrod.exceptions.HotRodClientException;
import org.infinispan.client.hotrod.jni.AdminFlagSet;
import org.infinispan.commons.configuration.BasicConfiguration;

public class RemoteCacheManagerAdminImpl implements RemoteCacheManagerAdmin {

   public RemoteCacheManagerAdminImpl(RemoteCacheManager remoteCacheManager, org.infinispan.client.hotrod.jni.RemoteCacheManagerAdmin admin) {
      this.remoteCacheManager = remoteCacheManager;
      this.jniAdmin = admin;
   }

   @Override
   public <K, V> RemoteCache<K, V> createCache(String name, String template) throws HotRodClientException {
      jniAdmin.adminCreateCache_jb_jb(name, template!=null ? template : new String());
      return remoteCacheManager.<K,V>getCache(name);
   }

   @Override
   public <K, V> RemoteCache<K, V> createCache(String name, BasicConfiguration configuration)
         throws HotRodClientException {
      jniAdmin.adminCreateCacheWithXml_jb_jb(name, configuration.toXMLString());
      return remoteCacheManager.<K,V>getCache(name);
   }

   @Override
   public <K, V> RemoteCache<K, V> getOrCreateCache(String name, String template) throws HotRodClientException {
      jniAdmin.adminGetOrCreateCache_jb_jb(name,  template!=null ? template : new String());
      return remoteCacheManager.<K,V>getCache(name);
   }

   @Override
   public <K, V> RemoteCache<K, V> getOrCreateCache(String name, BasicConfiguration configuration)
         throws HotRodClientException {
      jniAdmin.adminGetOrCreateCacheWithXml_jb_jb(name, configuration.toXMLString());
      return remoteCacheManager.<K,V>getCache(name);
   }


   @Override
   public void removeCache(String name) throws HotRodClientException {
      jniAdmin.removeCache(name);
   }

   @Override
   public void reindexCache(String name) throws HotRodClientException {
      jniAdmin.reindexCache(name);
   }

   private RemoteCacheManager remoteCacheManager;
   private org.infinispan.client.hotrod.jni.RemoteCacheManagerAdmin jniAdmin;

   @Override
   public RemoteCacheManagerAdmin withFlags(org.infinispan.commons.api.CacheContainerAdmin.AdminFlag... flags) {
      EnumSet<org.infinispan.commons.api.CacheContainerAdmin.AdminFlag> s = EnumSet.copyOf(Arrays.asList(flags));
      withFlags(s);
      return this;
   }

   @Override
   public void createCache(String name, String template, EnumSet<org.infinispan.client.hotrod.AdminFlag> flags)
         throws HotRodClientException {
         throw new UnsupportedOperationException();
   }

   @Override
   public RemoteCacheManagerAdmin withFlags(EnumSet<AdminFlag> flags) {
      AdminFlagSet fs = new AdminFlagSet();
      for (AdminFlag adminFlag : flags) {
         switch(adminFlag) {
            case PERMANENT: fs.insert(org.infinispan.client.hotrod.jni.AdminFlag.PERMANENT);
         }

      }
      jniAdmin.withFlags(fs);
      return this;
   }

}
