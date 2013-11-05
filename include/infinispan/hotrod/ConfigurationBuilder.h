#ifndef ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
#define ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H

#include <string>
#include <vector>

#include "infinispan/hotrod/defs.h"
#include "infinispan/hotrod/ImportExport.h"
#include "Builder.h"
#include "Configuration.h"
#include "ConnectionPoolConfigurationBuilder.h"
#include "ServerConfigurationBuilder.h"
#include "SslConfigurationBuilder.h"

namespace infinispan {
namespace hotrod {

/**
 * ConfigurationBuilder used to generate immutable Configuration objects that are in turn
 * used to configure RemoteCacheManager instances.
 *
 */
class HR_EXTERN ConfigurationBuilder
    : public Builder<Configuration>
{
  public:
    ConfigurationBuilder():
        m_connectionTimeout(60000),
        m_forceReturnValue(false),
        m_keySizeEstimate(64),
        m_pingOnStartup(true),
        m_protocolVersion(PROTOCOL_VERSION_12),
        m_socketTimeout(60000),
        m_tcpNoDelay(true),
        m_valueSizeEstimate(512),
        connectionPoolConfigurationBuilder(*this),
        sslConfigurationBuilder(*this)
      { };

    /**
     * Adds a server to this Configuration. ServerConfigurationBuilder is in turn used
     * to actually configure a server.
     *
     *\return ServerConfigurationBuilder instance to be used for server configuration
     */
    ServerConfigurationBuilder& addServer();

    /**
     * Adds multiple servers to this Configuration. ConfigurationBuilder is in turn used
     * to actually configure added servers as well as other configuration settings.
     *
     *\return ConfigurationBuilder instance to be used for server configuration
     */
    ConfigurationBuilder& addServers(std::string servers);

    /**
     * Configures connection pool.
     *
     *\return ConnectionPoolConfigurationBuilder instance to be used for pool configuration
     */
    ConnectionPoolConfigurationBuilder& connectionPool();

    /**
     * Configures underlying TCP connection timeout.
     *
     *\return ConfigurationBuilder instance to be used for configuration
     */
    ConfigurationBuilder& connectionTimeout(int connectionTimeout_);

    /**
     * Configures whether or not to force returning values on all cache operations
     * that optionally return a value.
     *
     *\return ConfigurationBuilder instance to be used for configuration
     */
    ConfigurationBuilder& forceReturnValues(bool forceReturnValues_);

    /**
     * Sets the marshalled size estimate for keys in the remote cache.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& keySizeEstimate(int keySizeEstimate_);

    /**
     * Sets whether to ping remote HotRod Infinispan servers before any cache operations are initiated.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& pingOnStartup(bool pingOnStartup_);

    /**
     * Sets the protocol version for this ConfigurationBuilder. Protocol version is either:
     * Configuration::PROTOCOL_VERSION_10 or Configuration::PROTOCOL_VERSION_11 or
     * Configuration::PROTOCOL_VERSION_12
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& protocolVersion(std::string protocolVersion_);

    /**
     * Sets the socket timeout for the underlying connections in this ConfigurationBuilder.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& socketTimeout(int socketTimeout_);

    /**
     * Returns SslConfigurationBuilder for SSL configurations of the underlying sockets.
     *
     *\return SslConfigurationBuilder instance to be used for SSL configuration
     */
    SslConfigurationBuilder& ssl();

    /**
     * Set tcpNoDelay for this ConfigurationBuilder.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& tcpNoDelay(bool tcpNoDelay_);

    /**
     * Sets the marshalled estimate of the values in this cache.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& valueSizeEstimate(int valueSizeEstimate_);

    /**
     * Build and returns an actual Configuration instance to be used for configuration of
     * RemoteCacheManager.
     *
     *\return Configuration instance to be used for  RemoteCacheManager configuration
     */
    Configuration build();

    /**
     * Same as build method create returns an actual Configuration instance to be used for
     * configuration of RemoteCacheManager.
     *
     *\return Configuration instance to be used for  RemoteCacheManager configuration
     */
    virtual Configuration create();

    /**
     * Reads the given Configuration objects and returns a ConfigurationBuilder
     * for further configuration or creation of the given Configuration object copies.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    virtual ConfigurationBuilder& read(Configuration& bean);

    /**
     * The default Hot Rod protocol version
     */
    static const char* PROTOCOL_VERSION_12;


  private:
    int m_connectionTimeout;
    bool m_forceReturnValue;
    int m_keySizeEstimate;
    bool m_pingOnStartup;
    std::string m_protocolVersion;
    std::vector<HR_SHARED_PTR<ServerConfigurationBuilder> > m_servers;
    int m_socketTimeout;
    bool m_tcpNoDelay;
    int m_valueSizeEstimate;

    ConnectionPoolConfigurationBuilder connectionPoolConfigurationBuilder;
    SslConfigurationBuilder sslConfigurationBuilder;
};

}} // namespace

#endif // ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
