

#include "hotrod/impl/configuration/Configuration.h"

namespace infinispan {
namespace hotrod {

const char* Configuration::PROTOCOL_VERSION_12 = "1.2";

Configuration::Configuration(std::string protocolVersion,
        const ConnectionPoolConfiguration& cpc,
        int connectionTimeout,
        bool forceReturnValue,
        int keySizeEstimate,
        bool pingOnStartup,
        std::vector<ServerConfiguration,std::allocator<ServerConfiguration> > serversConfiguration,
        int socketTimeout,
        SslConfiguration sslConfiguration,
        bool tcpNoDelay,
        int valueSizeEstimate) : connectionPoolConfiguration(cpc)
{
  Configuration::protocolVersion = Configuration::PROTOCOL_VERSION_12;
  //Configuration::connectionPoolConfiguration = connectionPoolConfiguration;
  Configuration::connectionTimeout = connectionTimeout;
  Configuration::forceReturnValue = forceReturnValue;
  Configuration::keySizeEstimate = keySizeEstimate;
  Configuration::pingOnStartup = pingOnStartup;
  Configuration::servers = serversConfiguration;
  Configuration::socketTimeout = socketTimeout;
  Configuration::sslConfiguration = sslConfiguration;
  Configuration::tcpNoDelay = tcpNoDelay;
  Configuration::valueSizeEstimate = valueSizeEstimate;
}

const std::string& Configuration::getProtocolVersion() const
{
  return Configuration::protocolVersion;
}

const ConnectionPoolConfiguration& Configuration::getConnectionPoolConfiguration() const
{
  return Configuration::connectionPoolConfiguration;
}

const int& Configuration::getConnectionTimeout() const
{
  return Configuration::connectionTimeout;
}

const bool& Configuration::isForceReturnValue() const
{
  return Configuration::forceReturnValue;
}

const int& Configuration::getKeySizeEstimate() const
{
  return Configuration::keySizeEstimate;
}

const bool& Configuration::isPingOnStartup() const
{
  return Configuration::pingOnStartup;
}

const std::vector<ServerConfiguration>& Configuration::getServersConfiguration() const
{
  return Configuration::servers;
}

const int& Configuration::getSocketTimeout() const
{
  return Configuration::socketTimeout;
}

const SslConfiguration& Configuration::getSslConfiguration() const
{
  return Configuration::sslConfiguration;
}

const bool& Configuration::isTcpNoDelay() const
{
  return Configuration::tcpNoDelay;
}

const int& Configuration::getValueSizeEstimate() const
{
  return Configuration::valueSizeEstimate;
}

}} /* namespace */
