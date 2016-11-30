package org.infinispan.client.hotrod.configuration;


/**
 * NearCacheConfiguration.
 *
 * @author Vittorio Rigamonti
 */

import org.infinispan.client.hotrod.jni.NearCacheMode;

public class NearCacheConfiguration {
   private final NearCacheMode mode;
   private final int maxEntries;
   private org.infinispan.client.hotrod.jni.NearCacheConfiguration jniNearCacheConfiguration;

   public org.infinispan.client.hotrod.jni.NearCacheConfiguration getJniNearCacheConfiguration() {
      return jniNearCacheConfiguration;
   }

   public NearCacheConfiguration(org.infinispan.client.hotrod.jni.NearCacheConfiguration jniNearCacheConfiguration) {
      super();
      this.jniNearCacheConfiguration = jniNearCacheConfiguration;
      mode=NearCacheMode.DISABLED;
      maxEntries=0;
   }

   NearCacheConfiguration(NearCacheMode mode, int maxEntries) {
      this.mode=mode;
      this.maxEntries=maxEntries;
      this.jniNearCacheConfiguration = new org.infinispan.client.hotrod.jni.NearCacheConfiguration(NearCacheMode.DISABLED, 0);
   }

   NearCacheMode getMode()
   {
      return mode;
   }

   int getMaxEntries()
   {
      return maxEntries;
   }

   @Override
   public String toString() {
      return "NearCacheConfiguration [mode=" + (mode==NearCacheMode.INVALIDATED ? "INVALIDATED" : "DISABLED") + ", maxEntries=" + maxEntries+"]";
   }
}
