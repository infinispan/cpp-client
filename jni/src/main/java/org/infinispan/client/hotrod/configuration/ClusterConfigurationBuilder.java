package org.infinispan.client.hotrod.configuration;

public class ClusterConfigurationBuilder {
    private org.infinispan.client.hotrod.jni.ClusterConfigurationBuilder ccb;
    private ConfigurationBuilder builder;
	public ClusterConfigurationBuilder(ConfigurationBuilder builder, org.infinispan.client.hotrod.jni.ClusterConfigurationBuilder ccb) {
		this.ccb=ccb;
		this.builder=builder;
	}
	public ClusterConfigurationBuilder addClusterNode(String host, int port)
	{
		ccb.addClusterNode(host, port);
		return this;
    }
}
