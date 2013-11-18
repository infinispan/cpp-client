#ifndef ISPN_HOTROD_CONFIGURATION_H
#define ISPN_HOTROD_CONFIGURATION_H



#include <string>
#include <vector>
#include "infinispan/hotrod/ImportExport.h"
#include "ConnectionPoolConfiguration.h"
#include "ServerConfiguration.h"
#include "SslConfiguration.h"

namespace infinispan {
namespace hotrod {

/**
 * Configuration object along with its factory ConfigurationBuilder represent
 * the preferred approach of configuring RemoteCacheManager.
 *
 */
class HR_EXTERN Configuration
{
  public:
    static const char* PROTOCOL_VERSION_10;
    static const char* PROTOCOL_VERSION_11;
    static const char* PROTOCOL_VERSION_12;

    Configuration(std::string protocolVersion,
            const ConnectionPoolConfiguration& connectionPoolConfiguration,
            int connectionTimeout,
            bool forceReturnValue,
            int keySizeEstimate,
            bool pingOnStartup,
            std::vector<ServerConfiguration> serversConfiguration,
            int socketTimeout,
            const SslConfiguration sslConfiguration,
            bool tcpNoDelay,
            int valueSizeEstimate);

    /**
     * Gets the protocol version for this Configuration. Protocol version is either:
     * Configuration::PROTOCOL_VERSION_10 or Configuration::PROTOCOL_VERSION_11 or
     * Configuration::PROTOCOL_VERSION_12
     *
     *\return String representation of the protocol version
     */
    const std::string& getProtocolVersion() const;

    /**
     * Gets the ConnectionPoolConfiguration instance for this Configuration.
     *
     *\return ConnectionPoolConfiguration for further connection pool configuration
     */
    const ConnectionPoolConfiguration& getConnectionPoolConfiguration() const;

    /**
     * Gets the connection timeout for this Configuration.
     *
     *\return connection timeout in milliseconds
     */

    const int& getConnectionTimeout() const;

    /**
     * Returns whether to force returning values on all cache operations
     * that optionally return a value.
     *
     *\return true if forcing return value is turned on, false otherwise
     */

    const bool& isForceReturnValue() const;

    /**
     * Returns the marshalled size estimate for keys in the remote cache.
     *
     *\return estimated size (bytes) of keys in remote cache
     */
    const int& getKeySizeEstimate() const;

    /**
     * Returns true if Hot Rod servers are going to pinged before any cache
     * %operations are invoked, false otherwise.
     *
     *\return true if servers are going to be pinged, false otherwise
     */
    const bool& isPingOnStartup() const;

    /**
     * Returns the vector of server configurations where each server configuration instance
     * describes a HotRod server address and port.
     *
     *\return vector of server configurations
     */
    const std::vector<ServerConfiguration>& getServersConfiguration() const;

    /**
     * Returns socket timeout of underlying TCP connection(s)
     *
     *\return socket timeout in milliseconds
     */
    const int& getSocketTimeout() const;

    /**
     *Returns SSL configuration for the underlying TCP connections
     *
     *\return SSL configuration
     */
    const SslConfiguration& getSslConfiguration() const;

    /**
     * Returns true if TCP no delay is turned on for underlying TCP connections
     *
     *\return true if TCP no delay is turned on
     */
    const bool& isTcpNoDelay() const;

    /**
     * Returns the marshalled size estimate for values in the remote cache.
     *
     *\return estimated size (bytes) of values in remote cache
     */
    const int& getValueSizeEstimate() const;

  private:
    std::string protocolVersion;
    ConnectionPoolConfiguration connectionPoolConfiguration;
    int connectionTimeout;
    bool forceReturnValue;
    int keySizeEstimate;
    bool pingOnStartup;
    std::vector<ServerConfiguration> servers;
    int socketTimeout;
    SslConfiguration sslConfiguration;
    bool tcpNoDelay;
    int valueSizeEstimate;
};

}} // namespace

#endif /* ISPN_HOTROD_CONFIGURATION_H */
