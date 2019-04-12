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
    ConfigurationChildBuilder(ConfigurationBuilder &builder): m_builder(&builder) {}

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
    ConfigurationBuilder& addServers(const std::string &servers);

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
    ConfigurationBuilder& connectionTimeout(int connectionTimeout);

    /**
     * Whether or not to implicitly FORCE_RETURN_VALUE for all calls.
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConfigurationBuilder& forceReturnValues(bool forceReturnValues);

    /**
     * This hint allows sizing of byte buffers when serializing and deserializing
     * keys, to minimize array resizing. It defaults to 64.
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConfigurationBuilder& keySizeEstimate(int keySizeEstimate);

    /**
     * This property defines the protocol version that this client should use. Defaults to 1.2. Other
     * valid values include 1.0 and 1.1.
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConfigurationBuilder& protocolVersion(const std::string &protocolVersion);

    /**
     * This property defines the maximum socket read timeout in milliseconds before giving up waiting
     * for bytes from the server. Defaults to 60000 (1 minute)
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConfigurationBuilder& socketTimeout(int socketTimeout);

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
    ConfigurationBuilder& tcpNoDelay(bool tcpNoDelay);

    /**
     * This hint allows sizing of byte buffers when serializing and deserializing values, to minimize
     * array resizing. It defaults to 512.
     *
     * \return ConfigurationBuilder for further configuration
     */
    ConfigurationBuilder& valueSizeEstimate(int valueSizeEstimate);

    /**
     * Sets the maximum number of retries for each request. A valid value should be greater or equals than 0 (zero).
     * Zero means no retry will made in case of a network failure. It defaults to 10.
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& maxRetries(int maxRetries_);

    /**
     * Sets the balancer generator function
     *
     *\return ConfigurationBuilder instance to be used for further configuration
     */
    ConfigurationBuilder& balancingStrategyProducer(FailOverRequestBalancingStrategy::ProducerFn bsp);

    Configuration build();

private:
    // in order to store this class in collections, operator= must work
    ConfigurationBuilder *m_builder;
};

}} // namespace

#endif // ISPN_HOTROD_CONFIGURATION_CONFIGURATIONCHILDBUILDER_H
