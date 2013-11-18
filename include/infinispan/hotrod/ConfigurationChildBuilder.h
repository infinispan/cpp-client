#ifndef ISPN_HOTROD_CONFIGURATION_CONFIGURATIONCHILDBUILDER_H
#define ISPN_HOTROD_CONFIGURATION_CONFIGURATIONCHILDBUILDER_H



#include <string>
#include "infinispan/hotrod/ImportExport.h"
#include "Configuration.h"

namespace infinispan {
namespace hotrod {

class ConfigurationBuilder;
class ConnectionPoolConfigurationBuilder;
class ServerConfigurationBuilder;
class SslConfigurationBuilder;

class HR_EXTERN ConfigurationChildBuilder
{
  public:
    ConfigurationChildBuilder(ConfigurationBuilder& builder_);

	/**
	 * Adds a new remote server
	 *
	 */
    ServerConfigurationBuilder& addServer();

    /**
     * Adds a list of remote servers in the form: host1[:port][;host2[:port]]...
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConfigurationBuilder& addServers(std::string servers);

    /**
     * Configures the connection pool
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConnectionPoolConfigurationBuilder& connectionPool();

    /**
     * This property defines the maximum socket connect timeout before giving up connecting to the
     * server.
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConfigurationBuilder& connectionTimeout(int connectionTimeout_);

    /**
     * Whether or not to implicitly FORCE_RETURN_VALUE for all calls.
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConfigurationBuilder& forceReturnValues(bool forceReturnValues_);

    /**
     * This hint allows sizing of byte buffers when serializing and deserializing
     * keys, to minimize array resizing. It defaults to 64.
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConfigurationBuilder& keySizeEstimate(int keySizeEstimate_);

    /**
     * If true, a ping request is sent to a back end server in order to fetch cluster's topology.
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConfigurationBuilder& pingOnStartup(bool pingOnStartup_);

    /**
     * This property defines the protocol version that this client should use. Defaults to 1.2. Other
     * valid values include 1.0 and 1.1.
     *
     * \return ConfigurationBuilder for further configuration
     * \sa Configuration::PROTOCOL_VERSION_10
     */
    ConfigurationBuilder& protocolVersion(std::string protocolVersion_);

    /**
     * This property defines the maximum socket read timeout in milliseconds before giving up waiting
     * for bytes from the server. Defaults to 60000 (1 minute)
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConfigurationBuilder& socketTimeout(int socketTimeout_);

    /**
     * SSL Configuration
     *
     * \return SslConfigurationBuilder for further SSL configuration
     */
    SslConfigurationBuilder& ssl();

    /**
     * Affects TCP NODELAY on the TCP stack. Defaults to enabled.
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConfigurationBuilder& tcpNoDelay(bool tcpNoDelay_);

    /**
     * This hint allows sizing of byte buffers when serializing and deserializing values, to minimize
     * array resizing. It defaults to 512.
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConfigurationBuilder& valueSizeEstimate(int valueSizeEstimate_);
    Configuration build();

  private:
    ConfigurationBuilder& m_builder;
};

}} // namespace

#endif // ISPN_HOTROD_CONFIGURATION_CONFIGURATIONCHILDBUILDER_H
