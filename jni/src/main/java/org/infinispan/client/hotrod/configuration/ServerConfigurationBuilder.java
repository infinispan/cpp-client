package org.infinispan.client.hotrod.configuration;

import org.infinispan.commons.configuration.Builder;

/**
 * ServerConfigurationBuilder.
 *
 * @author Tristan Tarrant
 * @since 5.3
 */
public class ServerConfigurationBuilder extends AbstractConfigurationChildBuilder implements Builder<ServerConfiguration> {
   
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

   @Override
   public void validate() {
      this.jniServerConfigurationBuilder.validate();
   }

   @Override
   public ServerConfiguration create() {
      return new ServerConfiguration(this.jniServerConfigurationBuilder.create());
   }

   @Override
   public ServerConfigurationBuilder read(ServerConfiguration template) {      
      this.jniServerConfigurationBuilder.host(template.host());
      this.jniServerConfigurationBuilder.port(template.port());

      return this;
   }

}
