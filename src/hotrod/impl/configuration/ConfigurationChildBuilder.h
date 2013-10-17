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

    ServerConfigurationBuilder& addServer();
    ConfigurationBuilder& addServers(std::string servers);
    ConnectionPoolConfigurationBuilder& connectionPool();
    ConfigurationBuilder& connectionTimeout(int connectionTimeout_);
    ConfigurationBuilder& forceReturnValues(bool forceReturnValues_);
    ConfigurationBuilder& keySizeEstimate(int keySizeEstimate_);
    ConfigurationBuilder& pingOnStartup(bool pingOnStartup_);
    ConfigurationBuilder& protocolVersion(std::string protocolVersion_);
    ConfigurationBuilder& socketTimeout(int socketTimeout_);
    SslConfigurationBuilder& ssl();
    ConfigurationBuilder& tcpNoDelay(bool tcpNoDelay_);
    ConfigurationBuilder& valueSizeEstimate(int valueSizeEstimate_);
    Configuration build();

  private:
    ConfigurationBuilder& m_builder;
};

}} // namespace

#endif // ISPN_HOTROD_CONFIGURATION_CONFIGURATIONCHILDBUILDER_H
