package org.infinispan.client.hotrod.configuration;

import javax.net.ssl.SSLContext;

/**
 * SslConfiguration.
 *
 * @author Tristan Tarrant
 * @since 5.3
 */
public class SslConfiguration {
   private final boolean enabled;
   private final String keyStoreFileName;
   private final char[] keyStorePassword;
   private final SSLContext sslContext;
   private final String trustStoreFileName;
   private final char[] trustStorePassword;

   private org.infinispan.client.hotrod.jni.SslConfiguration jniSslConfiguration;

   public org.infinispan.client.hotrod.jni.SslConfiguration getJniSslConfiguration() {
      return jniSslConfiguration;
   }

   public SslConfiguration(org.infinispan.client.hotrod.jni.SslConfiguration jniSslConfiguration) {
      super();
      this.jniSslConfiguration = jniSslConfiguration;
      this.enabled = false;
      this.keyStoreFileName = null;
      this.keyStorePassword = null;
      this.sslContext = null;
      this.trustStoreFileName = null;
      this.trustStorePassword = null;
   }

   SslConfiguration(boolean enabled, String keyStoreFileName, char[] keyStorePassword, SSLContext sslContext, String trustStoreFileName,
         char[] trustStorePassword) {
      this.enabled = enabled;
      this.keyStoreFileName = keyStoreFileName;
      this.keyStorePassword = keyStorePassword;
      this.sslContext = sslContext;
      this.trustStoreFileName = trustStoreFileName;
      this.trustStorePassword = trustStorePassword;
      this.jniSslConfiguration = new org.infinispan.client.hotrod.jni.SslConfiguration();
   }

   public boolean enabled() {
      return enabled;
   }

   public String keyStoreFileName() {
      return keyStoreFileName;
   }

   public char[] keyStorePassword() {
      return keyStorePassword;
   }

   public SSLContext sslContext() {
      return sslContext;
   }

   public String trustStoreFileName() {
      return trustStoreFileName;
   }

   public char[] trustStorePassword() {
      return trustStorePassword;
   }

   @Override
   public String toString() {
      return "SslConfiguration [enabled=" + enabled() + ", keyStoreFileName="
            + keyStoreFileName() + ", sslContext=" + sslContext() + ", trustStoreFileName=" + trustStoreFileName() + "]";
   }
}
