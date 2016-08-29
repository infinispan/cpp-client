package org.infinispan.client.hotrod.configuration;

import org.infinispan.commons.configuration.Builder;

/**
 * ServerConfigurationBuilder.
 *
 * @author Tristan Tarrant
 * @since 5.3
 */
public class ServerConfigurationBuilder extends AbstractConfigurationChildBuilder {
   
   private org.infinispan.client.hotrod.jni.ServerConfigurationBuilder jniServerConfigurationBuilder;

   ServerConfigurationBuilder(ConfigurationBuilder builder) {
      super(builder);
      jniServerConfigurationBuilder = builder.getJniConfigurationBuilder().addServer();
   }

   public ServerConfigurationBuilder host(String host) {
      this.jniServerConfigurationBuilder.host(host);
      return this;
   }

   public ServerConfigurationBuilder port(int port) {
      this.jniServerConfigurationBuilder.port(port);
      return this;
   }

   public ServerConfiguration create() {
      return new ServerConfiguration(this.jniServerConfigurationBuilder.create());
   }

   public ServerConfigurationBuilder read(ServerConfiguration template) {      
      this.jniServerConfigurationBuilder.host(template.host());
      this.jniServerConfigurationBuilder.port(template.port());

      return this;
   }

}
