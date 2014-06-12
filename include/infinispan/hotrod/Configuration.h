#ifndef ISPN_HOTROD_CONFIGURATION_H
#define ISPN_HOTROD_CONFIGURATION_H



#include <string>
#include <vector>
#include "infinispan/hotrod/portable.h"
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
// TODO: should we really return references?
class Configuration
{
  public:
    HR_EXTERN static const char* PROTOCOL_VERSION_10;
    HR_EXTERN static const char* PROTOCOL_VERSION_11;
    HR_EXTERN static const char* PROTOCOL_VERSION_12;

    Configuration(const std::string &_protocolVersion,
            const ConnectionPoolConfiguration& _connectionPoolConfiguration,
            int _connectionTimeout,
            bool _forceReturnValue,
            int _keySizeEstimate,
            bool _pingOnStartup,
            std::vector<ServerConfiguration> _serversConfiguration,
            int _socketTimeout,
            const SslConfiguration _sslConfiguration,
            bool _tcpNoDelay,
            int _valueSizeEstimate):
                protocolVersion(_protocolVersion), protocolVersionPtr(),
                connectionPoolConfiguration(_connectionPoolConfiguration),
                connectionTimeout(_connectionTimeout), forceReturnValue(_forceReturnValue),
                keySizeEstimate(_keySizeEstimate), pingOnStartup(_pingOnStartup), servers(_serversConfiguration),
                socketTimeout(_socketTimeout), sslConfiguration(_sslConfiguration),tcpNoDelay(_tcpNoDelay),
                valueSizeEstimate(_valueSizeEstimate) {}

    /**
     * DEPRECATED. Use getProtocolVersionCString().
     * In the future, this should return either const char * or std::string
     */
    HR_EXTERN const std::string &getProtocolVersion() const {
        if (protocolVersionPtr.get() == NULL) {
            const_cast<Configuration *>(this)->protocolVersionPtr
                    .set(new std::string(protocolVersion.c_string()), &deleteString);
        }
        return *(protocolVersionPtr.get());
    }

    /**
     * Gets the protocol version for this Configuration. Protocol version is either:
     * Configuration::PROTOCOL_VERSION_10 or Configuration::PROTOCOL_VERSION_11 or
     * Configuration::PROTOCOL_VERSION_12
     *
     *\return String representation of the protocol version
     */
    HR_EXTERN const char *getProtocolVersionCString() const;

    /**
     * Gets the ConnectionPoolConfiguration instance for this Configuration.
     *
     *\return ConnectionPoolConfiguration for further connection pool configuration
     */
    HR_EXTERN const ConnectionPoolConfiguration& getConnectionPoolConfiguration() const;

    /**
     * Gets the connection timeout for this Configuration.
     *
     *\return connection timeout in milliseconds
     */
    HR_EXTERN const int& getConnectionTimeout() const;

    /**
     * Returns whether to force returning values on all cache operations
     * that optionally return a value.
     *
     *\return true if forcing return value is turned on, false otherwise
     */

    HR_EXTERN const bool& isForceReturnValue() const;

    /**
     * Returns the marshalled size estimate for keys in the remote cache.
     *
     *\return estimated size (bytes) of keys in remote cache
     */
    HR_EXTERN const int& getKeySizeEstimate() const;

    /**
     * Returns true if Hot Rod servers are going to pinged before any cache
     * %operations are invoked, false otherwise.
     *
     *\return true if servers are going to be pinged, false otherwise
     */
    HR_EXTERN const bool& isPingOnStartup() const;

    /**
     * Returns the vector of server configurations where each server configuration instance
     * describes a HotRod server address and port.
     *
     *\return vector of server configurations
     */
    std::vector<ServerConfiguration> getServersConfiguration() const {
        return servers.std_vector();
    }

    /**
     * Returns socket timeout of underlying TCP connection(s)
     *
     *\return socket timeout in milliseconds
     */
    HR_EXTERN const int& getSocketTimeout() const;

    /**
     *Returns SSL configuration for the underlying TCP connections
     *
     *\return SSL configuration
     */
    HR_EXTERN const SslConfiguration& getSslConfiguration() const;

    /**
     * Returns true if TCP no delay is turned on for underlying TCP connections
     *
     *\return true if TCP no delay is turned on
     */
    HR_EXTERN const bool& isTcpNoDelay() const;

    /**
     * Returns the marshalled size estimate for values in the remote cache.
     *
     *\return estimated size (bytes) of values in remote cache
     */
    HR_EXTERN const int& getValueSizeEstimate() const;

private:
    portable::string protocolVersion;
    portable::local_ptr<std::string> protocolVersionPtr;
    ConnectionPoolConfiguration connectionPoolConfiguration;
    int connectionTimeout;
    bool forceReturnValue;
    int keySizeEstimate;
    bool pingOnStartup;
    portable::vector<ServerConfiguration> servers;
    int socketTimeout;
    SslConfiguration sslConfiguration;
    bool tcpNoDelay;
    int valueSizeEstimate;

    static void deleteString(std::string *str) { delete str; }
};

}} // namespace

#endif /* ISPN_HOTROD_CONFIGURATION_H */
