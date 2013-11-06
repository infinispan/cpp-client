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

    const std::string& getProtocolVersion() const;
    const ConnectionPoolConfiguration& getConnectionPoolConfiguration() const;
    const int& getConnectionTimeout() const;
    const bool& isForceReturnValue() const;
    const int& getKeySizeEstimate() const;
    const bool& isPingOnStartup() const;
    const std::vector<ServerConfiguration>& getServersConfiguration() const;
    const int& getSocketTimeout() const;
    const SslConfiguration& getSslConfiguration() const;
    const bool& isTcpNoDelay() const;
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
