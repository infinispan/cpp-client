#ifndef ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
#define ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include "infinispan/hotrod/defs.h"
#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/EventMarshaller.h"
#include "Configuration.h"
#include "ConnectionPoolConfigurationBuilder.h"
#include "ServerConfigurationBuilder.h"
#include "SecurityConfigurationBuilder.h"
#include "NearCacheConfiguration.h"

using namespace infinispan::hotrod::event;

namespace infinispan {
namespace hotrod {

/**
 * ClusterConfigurationBuilder is used to configure the clusters that are part of the system and
 * is a builder of ClusterConfiguration objects.
 */
class ClusterConfigurationBuilder
{
public:
	ClusterConfigurationBuilder(std::vector<ServerConfigurationBuilder>& servers, ConfigurationBuilder &parent) : servers(servers), m_parent(parent) {}
	/**
	 * Add a server node to this cluster
	 *
	 * \param host hostname
	 * \param port tcp port number
	 *
	 * \return this object for fluent configuration
	 */
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
 * Configuration classes for near cache subsystem
 */
class NearCacheConfigurationBuilder : public ConfigurationChildBuilder
{
public:
    NearCacheConfigurationBuilder(ConfigurationBuilder& _builder) : ConfigurationChildBuilder(_builder) {}

    /**
     * \return the max number of entries that the near cache can contain
     */
    int getMaxEntries() const {
        return m_maxEntries;
    }

    /**
     * Set the max number of entries that the near cache can contain
     *
     * \param maxEntries maximum number of entries
     *
     * \return this object for fluent configuration
     */
    NearCacheConfigurationBuilder& maxEntries(unsigned int maxEntries = 0) {
        this->m_maxEntries = maxEntries;
        return *this;
    }

    /**
     * \return the current working mode
     */
    NearCacheMode getMode() const {
        return m_mode;
    }

    /**
     * Set the working mode for the near cache
     *
     * \param mode the working mode (NearCacheMode)
     *
     * \return this object for fluent configuration
     */
    NearCacheConfigurationBuilder& mode(NearCacheMode mode = DISABLED) {
        this->m_mode = mode;
        return *this;
    }

    NearCacheConfiguration create()
    {
        return NearCacheConfiguration(m_mode,m_maxEntries);
    }

    private:
    NearCacheMode m_mode=DISABLED;
    unsigned int m_maxEntries=0;
};

/**
 * ConfigurationBuilder is used to generate immutable Configuration objects that are in turn
 * used to configure RemoteCacheManager instances.
 *
 */
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
        securityConfigurationBuilder(*this),
        nearCacheConfigurationBuilder(*this),
        m_transactional(false)
        {}

     void validate() {}

    /**
     *  Add a new cluster to the configuration builder
     *
     * \param clusterName the name of the new cluster
     *
     * \return a ClusterConfigurationBuilder object usable to configure the new cluster
     */
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
        return securityConfigurationBuilder.getSslConfigurationBuilder();
    }

    /**
     * Returns NearCacheConfigurationBuilder for near cache enabling and configuration.
     *
     *\return NearCacheConfigurationBuilder instance to be used for configuration
     */
    NearCacheConfigurationBuilder& nearCache() {
        return nearCacheConfigurationBuilder;
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
            securityConfigurationBuilder.create(),
            m_tcpNoDelay,
            m_valueSizeEstimate,
            m_maxRetries,
            nearCacheConfigurationBuilder.create(),
            m_balancingStrategyProducer,
            m_eventMarshaller, m_transactional);

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
        m_eventMarshaller = configuration.getEventMarshaller();
        m_transactional = configuration.isTransactional();
        return *this;
    }
    /**
     * Return the builder related to autentication and TLS
     *
     * \return a SecurityConfigurationBuilder object
     */
    SecurityConfigurationBuilder& security()
    {
        return securityConfigurationBuilder;
    }

    bool isTransactional() const
    {
        return m_transactional;
    }

    ConfigurationBuilder& setTransactional(bool transactional)
            {
        m_transactional = transactional;
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
    unsigned int m_maxRetries;
    FailOverRequestBalancingStrategy::ProducerFn m_balancingStrategyProducer;
    ConnectionPoolConfigurationBuilder connectionPoolConfigurationBuilder;
    SecurityConfigurationBuilder securityConfigurationBuilder;
    JBasicEventMarshaller m_defaultEventMarshaller;
    NearCacheConfigurationBuilder nearCacheConfigurationBuilder;
    bool m_transactional;

    EventMarshaller &m_eventMarshaller=m_defaultEventMarshaller;
};

}} // namespace

#endif // ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
