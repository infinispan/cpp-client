#ifndef ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
#define ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include "infinispan/hotrod/defs.h"
#include "infinispan/hotrod/ImportExport.h"
#include "Configuration.h"
#include "ConnectionPoolConfigurationBuilder.h"
#include "ServerConfigurationBuilder.h"
#include "SslConfigurationBuilder.h"

namespace infinispan {
namespace hotrod {
class ClusterConfigurationBuilder
{
public:
	ClusterConfigurationBuilder(std::vector<ServerConfigurationBuilder>& servers, ConfigurationBuilder &parent) : servers(servers), m_parent(parent) {}
	ClusterConfigurationBuilder& addClusterNode(const std::string host, const int port)
	{
        servers.push_back(ServerConfigurationBuilder(m_parent).host(host).port(port));
        return *this;
	}
private:
    std::vector<ServerConfigurationBuilder>& servers;
    ConfigurationBuilder &m_parent;
};
/**
 * ConfigurationBuilder used to generate immutable Configuration objects that are in turn
 * used to configure RemoteCacheManager instances.
 *
 */
// Methods on this class cannot be invoked across library boundary, therefore, it can use STL.
class ConfigurationBuilder
{
  public:
    ConfigurationBuilder():
        m_connectionTimeout(60000),
        m_forceReturnValue(false),
        m_keySizeEstimate(64),
        m_protocolVersion(Configuration::PROTOCOL_VERSION_24),
        m_socketTimeout(60000),
        m_tcpNoDelay(true),
        m_valueSizeEstimate(512),
        m_maxRetries(10),
		m_balancingStrategyProducer(nullptr),
        __pragma(warning(suppress:4355)) // passing uninitialized 'this'
        connectionPoolConfigurationBuilder(*this),
        __pragma(warning(suppress:4355))
        sslConfigurationBuilder(*this)
        {}

     void validate() {}

    ClusterConfigurationBuilder addCluster(const std::string& clusterName) {
    		return ClusterConfigurationBuilder(m_serversMap[clusterName],*this);
    }

    /**
    * Adds a server to this Configuration. ServerConfigurationBuilder is in turn used
    * to actually configure a server.
    *
    *\return ServerConfigurationBuilder instance to be used for server configuration
    */
   ServerConfigurationBuilder& addServer() {
		if (m_serversMap.find(Configuration::DEFAULT_CLUSTER_NAME) == m_serversMap.end())
	   {
			m_serversMap[Configuration::DEFAULT_CLUSTER_NAME];
	   }
		auto& servers = m_serversMap[Configuration::DEFAULT_CLUSTER_NAME];
   	   servers.push_back(ServerConfigurationBuilder(*this));
       return servers[servers.size() - 1];
   }


    /**
     * Adds multiple servers to this Configuration. ConfigurationBuilder is in turn used
     * to actually configure added servers as well as other configuration settings.
     *
     *\return ConfigurationBuilder instance to be used for server configuration
     */
    ConfigurationBuilder& addServers(std::string servers) {
        std::istringstream originalStream(servers);
        std::string stringHelper;
        std::string hostHelper;
        std::string portHelper;
        int portInt;

        // TODO: handle IPv6 addresses
        while(std::getline(originalStream,stringHelper,';')) {
            std::istringstream singleServerStream(stringHelper);
            std::getline(singleServerStream,hostHelper,':');
            std::getline(singleServerStream,portHelper,':');
            std::istringstream portStream(portHelper);
            portStream >> portInt;
            addServer().host(hostHelper).port(portInt);
        }

        return *this;
    }

    /**
     * Configures connection pool.
     *
     *\return ConnectionPoolConfigurationBuilder instance to be used for pool configuration
     */
    ConnectionPoolConfigurationBuilder& connectionPool() {
        return connectionPoolConfigurationBuilder;
    }

    /**
     * Configures underlying TCP connection timeout. Default is 60000 msec
     *
     *\return ConfigurationBuilder instance to be used for configuration
     */
    ConfigurationBuilder& connectionTimeout(int connectionTimeout_) {
        m_connectionTimeout = connectionTimeout_;
        return *this;
    }

    /**
     * Configures whether or not to force returning values on all cache operations
     * that optionally return a value. Default is false.
     *
     *\return ConfigurationBuilder instance to be used for configuration
     */
    ConfigurationBuilder& forceReturnValues(bool forceReturnValues_) {
        m_forceReturnValue = forceReturnValues_;
        return *this;
    }

    /**
     * Sets the marshalled size estimate for keys in the remote cache. Default is 64 bytes.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& keySizeEstimate(int keySizeEstimate_) {
        m_keySizeEstimate = keySizeEstimate_;
        return *this;
    }

    /**
     * Sets the protocol version for this ConfigurationBuilder. Protocol version is either:
     * Configuration::PROTOCOL_VERSION_10 or Configuration::PROTOCOL_VERSION_11 or
     * Configuration::PROTOCOL_VERSION_12. Default is PROTOCOL_VERSION_12.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& protocolVersion(const std::string &protocolVersion_) {
        m_protocolVersion = protocolVersion_;
        return *this;
    }

    /**
     * Sets the socket timeout for the underlying connections in this ConfigurationBuilder.
     * Default is 60000 msec.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& socketTimeout(int socketTimeout_) {
        m_socketTimeout = socketTimeout_;
        return *this;
    }

    /**
     * Returns SslConfigurationBuilder for SSL configurations of the underlying sockets.
     *
     *\return SslConfigurationBuilder instance to be used for SSL configuration
     */
    SslConfigurationBuilder& ssl() {
        return sslConfigurationBuilder;
    }

    /**
     * Set tcpNoDelay for this ConfigurationBuilder. Default is true.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& tcpNoDelay(bool tcpNoDelay_) {
        m_tcpNoDelay = tcpNoDelay_;
        return *this;
    }

    /**
     * Sets the marshalled estimate of the values in this cache. Default is 512 bytes.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& valueSizeEstimate(int valueSizeEstimate_) {
        m_valueSizeEstimate = valueSizeEstimate_;
        return *this;
    }

    /**
     * Sets the maximum number of retries for each request. A valid value should be greater or equals than 0 (zero).
     * Zero means no retry will made in case of a network failure. It defaults to 10.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& maxRetries(int maxRetries_) {
    	m_maxRetries = maxRetries_;
    	return *this;
    }

    ConfigurationBuilder& balancingStrategyProducer(FailOverRequestBalancingStrategy::ProducerFn bsp) {
        m_balancingStrategyProducer = bsp;
        return *this;
    }

    /**
     * Build and returns an actual Configuration instance to be used for configuration of
     * RemoteCacheManager.
     *
     *\return Configuration instance to be used for  RemoteCacheManager configuration
     */
    Configuration build() {
        return create();
    }

    /**
     * Same as build method create returns an actual Configuration instance to be used for
     * configuration of RemoteCacheManager.
     *
     *\return Configuration instance to be used for  RemoteCacheManager configuration
     */
    Configuration create() {
        std::map<std::string,std::vector<ServerConfiguration>> serversMap;
        for (auto p: m_serversMap)
        {
        	std::vector<ServerConfiguration> scVec;
        	for (auto e : p.second)
        	{
        		scVec.push_back(e.create());
        	}
        	serversMap.insert(std::make_pair(p.first, scVec));
        }
        if (serversMap.size()==0)
        {
        	std::vector<ServerConfiguration> scVec;
        	scVec.push_back(ServerConfigurationBuilder(*this).create());
			serversMap.insert(std::make_pair(Configuration::DEFAULT_CLUSTER_NAME, scVec));
        }

        return Configuration(m_protocolVersion,
            connectionPoolConfigurationBuilder.create(),
            m_connectionTimeout,
            m_forceReturnValue,
            m_keySizeEstimate,
            serversMap,
            m_socketTimeout,
            sslConfigurationBuilder.create(),
            m_tcpNoDelay,
            m_valueSizeEstimate,
            m_maxRetries,
            m_balancingStrategyProducer);

    }

    /**
     * Reads the given Configuration objects and returns a ConfigurationBuilder
     * for further configuration or creation of the given Configuration object copies.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& read(Configuration& configuration) {
        // FIXME: read pool, ssl and server configs
        m_protocolVersion = configuration.getProtocolVersionCString();
        m_connectionTimeout = configuration.getConnectionTimeout();
        m_forceReturnValue = configuration.isForceReturnValue();
        m_socketTimeout = configuration.getSocketTimeout();
        m_tcpNoDelay = configuration.isTcpNoDelay();
        m_keySizeEstimate = configuration.getKeySizeEstimate();
        m_valueSizeEstimate = configuration.getValueSizeEstimate();
        m_maxRetries = configuration.getMaxRetries();
        return *this;
    }

  private:
    int m_connectionTimeout;
    bool m_forceReturnValue;
    int m_keySizeEstimate;
    std::string m_protocolVersion;
    std::map<std::string,std::vector<ServerConfigurationBuilder> >m_serversMap;
    int m_socketTimeout;
    bool m_tcpNoDelay;
    int m_valueSizeEstimate;
    int m_maxRetries;
    FailOverRequestBalancingStrategy::ProducerFn m_balancingStrategyProducer;
    ConnectionPoolConfigurationBuilder connectionPoolConfigurationBuilder;
    SslConfigurationBuilder sslConfigurationBuilder;
};

}} // namespace

#endif // ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
