#include "infinispan/hotrod/Configuration.h"

namespace infinispan {
namespace hotrod {

const char* Configuration::PROTOCOL_VERSION_10 = "1.0";
const char* Configuration::PROTOCOL_VERSION_11 = "1.1";
const char* Configuration::PROTOCOL_VERSION_12 = "1.2";

const char *Configuration::getProtocolVersionCString() const
{
  return Configuration::protocolVersion.c_string();
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
