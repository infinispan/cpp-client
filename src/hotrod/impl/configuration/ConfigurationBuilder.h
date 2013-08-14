#ifndef ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
#define ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H



#include <string>
#include <vector>
#include "infinispan/hotrod/ImportExport.h"
#include "Configuration.h"
#include "hotrod/impl/configuration/Builder.h"
#include "ConnectionPoolConfigurationBuilder.h"
#include "ServerConfigurationBuilder.h"
#include "SslConfigurationBuilder.h"

namespace infinispan {
namespace hotrod {


class HR_EXTERN ConfigurationBuilder
    : public infinispan::hotrod::Builder<Configuration>
{
  public:
    ConfigurationBuilder();
    ServerConfigurationBuilder& addServer();
    ConfigurationBuilder& addServers(std::string servers);
    ConnectionPoolConfigurationBuilder& connectionPool();
    ConfigurationBuilder& connectionTiemout(int connectionTiemoutParam);
    ConfigurationBuilder& forceReturnValues(bool forceReturnValuesParam);
    ConfigurationBuilder& keySizeEstimate(int keySizeEstimateParam);
    ConfigurationBuilder& pingOnStartup(bool pingOnStartupParam);
    ConfigurationBuilder& protocolVersion(std::string protocolVersionParam);
    ConfigurationBuilder& socketTimeout(int socketTimeoutParam);
    SslConfigurationBuilder& ssl();
    ConfigurationBuilder& tcpNoDelay(bool tcpNoDelayParam);
    ConfigurationBuilder& valueSizeEstimate(int valueSizeEstimateParam);
    Configuration build();
    virtual Configuration create();
    virtual ConfigurationBuilder& read(Configuration& bean);
    static const char* PROTOCOL_VERSION_12;


  private:
    ConnectionPoolConfigurationBuilder connectionPoolConfigurationBuilder;
    int internalConnectionTimeout;
    bool internalForceReturnValue;
    int internalKeySizeEstimate;
    bool internalPingOnStartup;
    std::string internalProtocolVersion;
    std::vector<ServerConfigurationBuilder*> internalServers;
    int internalSocketTimeout;
    SslConfigurationBuilder sslConfigurationBuilder;
    bool internalTcpNoDelay;
    int internalValueSizeEstimate;

};

}} // namespace

#endif // ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
