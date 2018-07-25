#ifndef ISPN_HOTROD_CONFIGURATION_H
#define ISPN_HOTROD_CONFIGURATION_H



#include <string>
#include <vector>
#include <map>
#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/ConnectionPoolConfiguration.h"
#include "infinispan/hotrod/ServerConfiguration.h"
#include "infinispan/hotrod/SecurityConfiguration.h"
#include "infinispan/hotrod/NearCacheConfiguration.h"
#include "infinispan/hotrod/FailOverRequestBalancingStrategy.h"
#include "infinispan/hotrod/JBasicEventMarshaller.h"


namespace infinispan {
namespace hotrod {

/**
 * Configuration object along with its builder represent
 * the preferred approach of configuring RemoteCacheManager. Usually applications configure
 * an ConfigurationBuilder object and build a Configuration from it invoking ConfigurationBuilder.create().
 *
 */
// TODO: should we really return references?
class Configuration
{
  public:
    HR_EXTERN static const char* PROTOCOL_VERSION_10;
    HR_EXTERN static const char* PROTOCOL_VERSION_11;
    HR_EXTERN static const char* PROTOCOL_VERSION_12;
    HR_EXTERN static const char* PROTOCOL_VERSION_13;
    HR_EXTERN static const char* PROTOCOL_VERSION_20;
    HR_EXTERN static const char* PROTOCOL_VERSION_21;
    HR_EXTERN static const char* PROTOCOL_VERSION_22;
    HR_EXTERN static const char* PROTOCOL_VERSION_23;
    HR_EXTERN static const char* PROTOCOL_VERSION_24;
    HR_EXTERN static const char* PROTOCOL_VERSION_25;
    HR_EXTERN static const char* PROTOCOL_VERSION_26;
    HR_EXTERN static const char* PROTOCOL_VERSION_27;
    HR_EXTERN static const char* DEFAULT_CLUSTER_NAME;


    Configuration(const std::string &_protocolVersion,
            const ConnectionPoolConfiguration& _connectionPoolConfiguration,
            int _connectionTimeout,
            bool _forceReturnValue,
            int _keySizeEstimate,
            std::map<std::string,std::vector<ServerConfiguration> > _serversConfiguration,
            int _socketTimeout,
            const SslConfiguration _sslConfiguration,
            bool _tcpNoDelay,
            int _valueSizeEstimate,
            int _maxRetries,
            NearCacheConfiguration _nearCacheConfiguration,
            FailOverRequestBalancingStrategy::ProducerFn bsp=0,
			const event::EventMarshaller &eventMarshaller = event::JBasicEventMarshaller(), bool transactional=false):
                protocolVersion(_protocolVersion), protocolVersionPtr(),
                connectionPoolConfiguration(_connectionPoolConfiguration),
                connectionTimeout(_connectionTimeout), forceReturnValue(_forceReturnValue),
                keySizeEstimate(_keySizeEstimate),
                serversMap(_serversConfiguration),
                socketTimeout(_socketTimeout), securityConfiguration(_sslConfiguration),tcpNoDelay(_tcpNoDelay),
                valueSizeEstimate(_valueSizeEstimate), maxRetries(_maxRetries), nearCacheConfiguration(_nearCacheConfiguration), balancingStrategyProducer(bsp),
				eventMarshaller(eventMarshaller), transactional(transactional)
    {}

    Configuration(const std::string &_protocolVersion,
            const ConnectionPoolConfiguration& _connectionPoolConfiguration,
            int _connectionTimeout,
            bool _forceReturnValue,
            int _keySizeEstimate,
            std::map<std::string,std::vector<ServerConfiguration> > _serversConfiguration,
            int _socketTimeout,
            const SecurityConfiguration _securityConfiguration,
            bool _tcpNoDelay,
            int _valueSizeEstimate,
            int _maxRetries,
            NearCacheConfiguration _nearCacheConfiguration,
            FailOverRequestBalancingStrategy::ProducerFn bsp=0,
            const event::EventMarshaller &eventMarshaller = event::JBasicEventMarshaller(), bool transactional=false):
                protocolVersion(_protocolVersion), protocolVersionPtr(),
                connectionPoolConfiguration(_connectionPoolConfiguration),
                connectionTimeout(_connectionTimeout), forceReturnValue(_forceReturnValue),
                keySizeEstimate(_keySizeEstimate),
                serversMap(_serversConfiguration),
                socketTimeout(_socketTimeout), securityConfiguration(_securityConfiguration),tcpNoDelay(_tcpNoDelay),
                valueSizeEstimate(_valueSizeEstimate), maxRetries(_maxRetries), nearCacheConfiguration(_nearCacheConfiguration), balancingStrategyProducer(bsp),
                eventMarshaller(eventMarshaller), transactional(transactional)
    {}


    /**
     * DEPRECATED. Use getProtocolVersionCString().
     * In the future, this should return either const char * or std::string
     */
    HR_EXTERN const std::string &getProtocolVersion() const {
        if (protocolVersionPtr.get() == NULL) {
            const_cast<Configuration *>(this)->protocolVersionPtr
                    .reset(new std::string(protocolVersion.c_str()));
        }
        return *(protocolVersionPtr.get());
    }

    /**
     * Gets the protocol version for this Configuration.
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
     * Returns the vector of the failover server configurations where each server configuration instance
     * describes a HotRod server address and port.
     *
     *\return vector of server configurations
     */
    std::map<std::string, std::vector<ServerConfiguration> > getServersMapConfiguration() const {
    	std::map<std::string, std::vector<ServerConfiguration> > temp;
        return serversMap;
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

    /**
     * Returns the maximum number of retries for each request.
     *
     *\return the maximum number of retries for each request.
     */
    HR_EXTERN const int& getMaxRetries() const;

    HR_EXTERN FailOverRequestBalancingStrategy::ProducerFn getBalancingStrategy() const;

    const SslConfiguration& getSslConfiguration() { return securityConfiguration.getSslConfiguration(); }

    HR_EXTERN const event::EventMarshaller &getEventMarshaller() const;

    const NearCacheConfiguration& getNearCacheConfiguration() const { return nearCacheConfiguration; }

    const SecurityConfiguration& getSecurityConfiguration() const { return securityConfiguration; }

    bool isTransactional() const { return transactional; }

    void setTransactional(bool transactional) { this->transactional = transactional; }

private:
    std::string protocolVersion;
    std::shared_ptr<std::string> protocolVersionPtr;
    ConnectionPoolConfiguration connectionPoolConfiguration;
    int connectionTimeout;
    bool forceReturnValue;
    int keySizeEstimate;
    std::map<std::string,std::vector<ServerConfiguration> > serversMap;
    int socketTimeout;
    SecurityConfiguration securityConfiguration;
    bool tcpNoDelay;
    int valueSizeEstimate;
    int maxRetries;
    const NearCacheConfiguration nearCacheConfiguration;
    FailOverRequestBalancingStrategy::ProducerFn balancingStrategyProducer;
    const event::EventMarshaller &eventMarshaller;
    bool transactional;

    static void deleteString(std::string *str) { delete str; }
};

}} // namespace

#endif /* ISPN_HOTROD_CONFIGURATION_H */
