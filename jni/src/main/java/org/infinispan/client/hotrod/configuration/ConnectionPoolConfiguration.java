package org.infinispan.client.hotrod.configuration;

/**
 * ConnectionPoolConfiguration.
 *
 * @author Tristan Tarrant
 * @since 5.3
 */
public class ConnectionPoolConfiguration {
   private ExhaustedAction exhaustedAction;
   
   private org.infinispan.client.hotrod.jni.ConnectionPoolConfiguration jniConnectionPoolConfiguration;

   public ConnectionPoolConfiguration(
         org.infinispan.client.hotrod.jni.ConnectionPoolConfiguration jniConnectionPoolConfiguration) {
      this.jniConnectionPoolConfiguration = jniConnectionPoolConfiguration;
      this.exhaustedAction.setExhaustedAction(this.jniConnectionPoolConfiguration.getExhaustedAction());
   }
   
   public org.infinispan.client.hotrod.jni.ConnectionPoolConfiguration getJniConnectionPoolConfiguration() {
      return this.jniConnectionPoolConfiguration;
   }

   public ExhaustedAction exhaustedAction() {
      this.exhaustedAction.setExhaustedAction(this.jniConnectionPoolConfiguration.getExhaustedAction());
      return this.exhaustedAction;
   }

   public boolean lifo() {
      return this.jniConnectionPoolConfiguration.isLifo();
   }

   public int maxActive() {
      return this.jniConnectionPoolConfiguration.getMaxActive();
   }

   public int maxTotal() {
      return this.jniConnectionPoolConfiguration.getMaxTotal();
   }

   public long maxWait() {
      return this.jniConnectionPoolConfiguration.getMaxWait();
   }

   public int maxIdle() {
      return this.jniConnectionPoolConfiguration.getMaxIdle();
   }

   public int minIdle() {
      return this.jniConnectionPoolConfiguration.getMinIdle();
   }

   public int numTestsPerEvictionRun() {
      return this.jniConnectionPoolConfiguration.getNumTestsPerEvictionRun();
   }

   public long timeBetweenEvictionRuns() {
      return this.jniConnectionPoolConfiguration.getTimeBetweenEvictionRuns();
   }

   public long minEvictableIdleTime() {
      return this.jniConnectionPoolConfiguration.getMinEvictableIdleTime();
   }

   public boolean testOnBorrow() {
      return this.jniConnectionPoolConfiguration.isTestOnBorrow();
   }

   public boolean testOnReturn() {
      return this.jniConnectionPoolConfiguration.isTestOnReturn();
   }

   public boolean testWhileIdle() {
      return this.jniConnectionPoolConfiguration.isTestWhileIdle();
   }

   @Override
   public String toString() {
      return "ConnectionPoolConfiguration [exhaustedAction=" + exhaustedAction() + ", lifo=" + lifo() + ", maxActive=" + maxActive() + ", maxTotal=" + maxTotal() + ", maxWait=" + maxWait()
            + ", maxIdle=" + maxIdle() + ", minIdle=" + minIdle() + ", numTestsPerEvictionRun=" + numTestsPerEvictionRun() + ", timeBetweenEvictionRuns=" + timeBetweenEvictionRuns()
            + ", minEvictableIdleTime=" + minEvictableIdleTime() + ", testOnBorrow=" + testOnBorrow() + ", testOnReturn=" + testOnReturn() + ", testWhileIdle=" + testWhileIdle() + "]";
   }

}
