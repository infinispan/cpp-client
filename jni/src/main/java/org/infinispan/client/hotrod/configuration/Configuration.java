package org.infinispan.client.hotrod.configuration;

import java.lang.ref.WeakReference;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import org.infinispan.client.hotrod.impl.consistenthash.ConsistentHash;
import org.infinispan.client.hotrod.impl.transport.TransportFactory;
import org.infinispan.client.hotrod.impl.transport.tcp.RequestBalancingStrategy;
import org.infinispan.client.hotrod.configuration.ConnectionPoolConfiguration;
import org.infinispan.client.hotrod.configuration.SslConfiguration;
import org.infinispan.commons.configuration.BuiltBy;
import org.infinispan.commons.marshall.Marshaller;

/**
 * Configuration.
 *
 * @author Tristan Tarrant
 * @since 5.3
 */
@BuiltBy(ConfigurationBuilder.class)
public class Configuration {

   private final ExecutorFactoryConfiguration asyncExecutorFactory;
   private final Class<? extends RequestBalancingStrategy> balancingStrategy;
   private final WeakReference<ClassLoader> classLoader;
//   private final ConnectionPoolConfiguration connectionPool;
//   private final int connectionTimeout;
   private final Class<? extends ConsistentHash>[] consistentHashImpl;
//   private final boolean forceReturnValues;
//   private final int keySizeEstimate;
   private final Class<? extends Marshaller> marshallerClass;
   private final Marshaller marshaller;
//   private final boolean pingOnStartup;
//   private final String protocolVersion;
   private final List<ServerConfiguration> servers;
//   private final int socketTimeout;
//   private final SslConfiguration ssl;
//   private final boolean tcpNoDelay;
   private final Class<? extends TransportFactory> transportFactory;
//   private final int valueSizeEstimate;
   
   private org.infinispan.client.hotrod.jni.Configuration jniConfiguration;

   public Configuration(org.infinispan.client.hotrod.jni.Configuration jniConfiguration) {
      super();
      this.jniConfiguration = jniConfiguration;
      this.asyncExecutorFactory = null;
      this.balancingStrategy = null;
      this.classLoader = null;
      this.consistentHashImpl = null;
      this.marshallerClass = null;
      this.marshaller = null;
      this.servers = null;
      this.transportFactory = null;
   }

   Configuration(ExecutorFactoryConfiguration asyncExecutorFactory, Class<? extends RequestBalancingStrategy> balancingStrategy, ClassLoader classLoader,
         ConnectionPoolConfiguration connectionPool, int connectionTimeout, Class<? extends ConsistentHash>[] consistentHashImpl, boolean forceReturnValues, int keySizeEstimate, Class<? extends Marshaller> marshallerClass,
         boolean pingOnStartup, String protocolVersion, List<ServerConfiguration> servers, int socketTimeout, SslConfiguration ssl, boolean tcpNoDelay,
         Class<? extends TransportFactory> transportFactory, int valueSizeEstimate) {
      this.jniConfiguration = new org.infinispan.client.hotrod.jni.Configuration(protocolVersion, connectionPool.getJniConnectionPoolConfiguration(),
            connectionTimeout, forceReturnValues, keySizeEstimate, pingOnStartup, null, socketTimeout, ssl.getJniSslConfiguration(), tcpNoDelay,
            valueSizeEstimate);
      this.asyncExecutorFactory = asyncExecutorFactory;
      this.balancingStrategy = balancingStrategy;
      this.classLoader = new WeakReference<ClassLoader>(classLoader);
      this.consistentHashImpl = consistentHashImpl;
      this.marshallerClass = marshallerClass;
      this.marshaller = null;
      this.servers = Collections.unmodifiableList(servers);
      this.transportFactory = transportFactory;
   }

   Configuration(ExecutorFactoryConfiguration asyncExecutorFactory, Class<? extends RequestBalancingStrategy> balancingStrategy, ClassLoader classLoader,
         ConnectionPoolConfiguration connectionPool, int connectionTimeout, Class<? extends ConsistentHash>[] consistentHashImpl, boolean forceReturnValues, int keySizeEstimate, Marshaller marshaller,
         boolean pingOnStartup, String protocolVersion, List<ServerConfiguration> servers, int socketTimeout, SslConfiguration ssl, boolean tcpNoDelay,
         Class<? extends TransportFactory> transportFactory, int valueSizeEstimate) {
      this.jniConfiguration = new org.infinispan.client.hotrod.jni.Configuration(protocolVersion, connectionPool.getJniConnectionPoolConfiguration(),
            connectionTimeout, forceReturnValues, keySizeEstimate, pingOnStartup, null, socketTimeout, ssl.getJniSslConfiguration(), tcpNoDelay,
            valueSizeEstimate);
      this.asyncExecutorFactory = asyncExecutorFactory;
      this.balancingStrategy = balancingStrategy;
      this.classLoader = new WeakReference<ClassLoader>(classLoader);
      this.consistentHashImpl = consistentHashImpl;
      this.marshallerClass = null;
      this.marshaller = marshaller;
      this.servers = Collections.unmodifiableList(servers);
      this.transportFactory = transportFactory;
   }

   public ExecutorFactoryConfiguration asyncExecutorFactory() {
      return asyncExecutorFactory;
   }

   public Class<? extends RequestBalancingStrategy> balancingStrategy() {
      return balancingStrategy;
   }

   public ClassLoader classLoader() {
      return classLoader.get();
   }

   public ConnectionPoolConfiguration connectionPool() {
      return new ConnectionPoolConfiguration(this.jniConfiguration.getConnectionPoolConfiguration());
   }

   public int connectionTimeout() {
      return this.jniConfiguration.getConnectionTimeout();
   }

   public Class<? extends ConsistentHash>[] consistentHashImpl() {
      return consistentHashImpl;
   }

   public Class<? extends ConsistentHash> consistentHashImpl(int version) {
      return consistentHashImpl[version-1];
   }

   public boolean forceReturnValues() {
      return this.jniConfiguration.isForceReturnValue();
   }

   public int keySizeEstimate() {
      return this.jniConfiguration.getKeySizeEstimate();
   }

   public Marshaller marshaller() {
      return marshaller;
   }

   public Class<? extends Marshaller> marshallerClass() {
      return marshallerClass;
   }

   public boolean pingOnStartup() {
      return this.jniConfiguration.isPingOnStartup();
   }

   public String protocolVersion() {
      return this.jniConfiguration.getProtocolVersion();
   }

   public List<ServerConfiguration> servers() {
      return servers;
   }

   public int socketTimeout() {
      return this.jniConfiguration.getSocketTimeout();
   }

   public SslConfiguration ssl() {
      return new SslConfiguration(this.jniConfiguration.getSslConfiguration());
   }

   public boolean tcpNoDelay() {
      return this.jniConfiguration.isTcpNoDelay();
   }

   public Class<? extends TransportFactory> transportFactory() {
      return transportFactory;
   }

   public int valueSizeEstimate() {
      return this.jniConfiguration.getValueSizeEstimate();
   }

   @Override
   public String toString() {
      return "Configuration [asyncExecutorFactory=" + asyncExecutorFactory + ", balancingStrategy=" + balancingStrategy + ", classLoader=" + classLoader + ", connectionPool="
            + connectionPool() + ", connectionTimeout=" + connectionTimeout() + ", consistentHashImpl=" + Arrays.toString(consistentHashImpl) + ", forceReturnValues="
            + forceReturnValues() + ", keySizeEstimate=" + keySizeEstimate() + ", marshallerClass=" + marshallerClass + ", marshaller=" + marshaller + ", pingOnStartup="
            + pingOnStartup() + ", protocolVersion=" + protocolVersion() + ", servers=" + servers + ", socketTimeout=" + socketTimeout() + ", ssl=" + ssl() + ", tcpNoDelay=" + tcpNoDelay()
            + ", transportFactory=" + transportFactory + ", valueSizeEstimate=" + valueSizeEstimate() + "]";
   }
}
