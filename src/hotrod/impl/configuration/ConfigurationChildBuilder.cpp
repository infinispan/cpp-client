#include "infinispan/hotrod/ConfigurationChildBuilder.h"
#include "infinispan/hotrod/ConfigurationBuilder.h"


namespace infinispan {
namespace hotrod {


/**
 * Adds a new remote server
 *
 */
ServerConfigurationBuilder& ConfigurationChildBuilder::addServer()
{
	return m_builder->addServer();
}

/**
 * Adds a list of remote servers in the form: host1[:port][;host2[:port]]...
 *
 * \return ConfigurationBuilder for further configuration
 */
ConfigurationBuilder& ConfigurationChildBuilder::addServers(const std::string &servers)
{
	return m_builder->addServers(servers);
}
/**
 * Configures the connection pool
 *
 * \return ConfigurationBuilder for further configuration
 */
ConnectionPoolConfigurationBuilder& ConfigurationChildBuilder::connectionPool()
{
	return m_builder->connectionPool();
}

/**
 * This property defines the maximum socket connect timeout before giving up connecting to the
 * server.
 *
 * \return ConfigurationBuilder for further configuration
 */
ConfigurationBuilder& ConfigurationChildBuilder::connectionTimeout(int connectionTimeout)
{
	return m_builder->connectionTimeout(connectionTimeout);
}

/**
 * Whether or not to implicitly FORCE_RETURN_VALUE for all calls.
 *
 * \return ConfigurationBuilder for further configuration
 */
ConfigurationBuilder& ConfigurationChildBuilder::forceReturnValues(bool forceReturnValues)
{
	return m_builder->forceReturnValues(forceReturnValues);
}

/**
 * This hint allows sizing of byte buffers when serializing and deserializing
 * keys, to minimize array resizing. It defaults to 64.
 *
 * \return ConfigurationBuilder for further configuration
 */
ConfigurationBuilder& ConfigurationChildBuilder::keySizeEstimate(int keySizeEstimate)
{
	return m_builder->keySizeEstimate(keySizeEstimate);
}

/**
 * This property defines the protocol version that this client should use. Defaults to 1.2. Other
 * valid values include 1.0 and 1.1.
 *
 * \return ConfigurationBuilder for further configuration
 * \sa Configuration::PROTOCOL_VERSION_10
 */
ConfigurationBuilder& ConfigurationChildBuilder::protocolVersion(const std::string &protocolVersion)
{
	return m_builder->protocolVersion(protocolVersion);
}

/**
 * This property defines the maximum socket read timeout in milliseconds before giving up waiting
 * for bytes from the server. Defaults to 60000 (1 minute)
 *
 * \return ConfigurationBuilder for further configuration
 */
ConfigurationBuilder& ConfigurationChildBuilder::socketTimeout(int socketTimeout)
{
	return m_builder->socketTimeout(socketTimeout);
}

/**
 * SSL Configuration
 *
 * \return SslConfigurationBuilder for further SSL configuration
 */
SslConfigurationBuilder& ConfigurationChildBuilder::ssl()
{
	return m_builder->ssl();
}

/**
 * Affects TCP NODELAY on the TCP stack. Defaults to enabled.
 *
 * \return ConfigurationBuilder for further configuration
 */
ConfigurationBuilder& ConfigurationChildBuilder::tcpNoDelay(bool tcpNoDelay)
{
	return m_builder->tcpNoDelay(tcpNoDelay);
}

/**
 * This hint allows sizing of byte buffers when serializing and deserializing values, to minimize
 * array resizing. It defaults to 512.
 *
 * \return ConfigurationBuilder for further configuration
 */
ConfigurationBuilder& ConfigurationChildBuilder::valueSizeEstimate(int valueSizeEstimate)
{
	return m_builder->valueSizeEstimate(valueSizeEstimate);
}

/**
 * Sets the maximum number of retries for each request. A valid value should be greater or equals than 0 (zero).
 * Zero means no retry will made in case of a network failure. It defaults to 10.
 *
 *\return ConfigurationBuilder instance to be used for further configuration
 */
ConfigurationBuilder& ConfigurationChildBuilder::maxRetries(int maxRetries_)
{
	return m_builder->maxRetries(maxRetries_);
}

/**
 * Sets the balancer generator function
 *
 *\return ConfigurationBuilder instance to be used for further configuration
 */
ConfigurationBuilder& ConfigurationChildBuilder::balancingStrategyProducer(FailOverRequestBalancingStrategy::ProducerFn bsp)
{
	return m_builder->balancingStrategyProducer(bsp);
}

Configuration ConfigurationChildBuilder::build()
{
	return m_builder->build();
}
}}
