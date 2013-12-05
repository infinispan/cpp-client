package org.infinispan.client.hotrod.configuration;

/**
 * ServerConfiguration.
 *
 * @author Tristan Tarrant
 * @since 5.3
 */
public class ServerConfiguration {
   
   private org.infinispan.client.hotrod.jni.ServerConfiguration jniServerConfiguration;

   public ServerConfiguration(org.infinispan.client.hotrod.jni.ServerConfiguration jniServerConfiguration) {
      this.jniServerConfiguration = jniServerConfiguration;
   }

   ServerConfiguration(String host, int port) {
      this.jniServerConfiguration = new org.infinispan.client.hotrod.jni.ServerConfiguration(host, port);
   }

   public String host() {
      return this.jniServerConfiguration.getHost();
   }

   public int port() {
      return this.jniServerConfiguration.getPort();
   }

}
