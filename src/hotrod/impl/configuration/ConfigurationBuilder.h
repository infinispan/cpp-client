#ifndef ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
#define ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H

#include <string>
#include <vector>

#include "infinispan/hotrod/ImportExport.h"
#include "Builder.h"
#include "Configuration.h"
#include "ConnectionPoolConfigurationBuilder.h"
#include "ServerConfigurationBuilder.h"
#include "SslConfigurationBuilder.h"

namespace infinispan {
namespace hotrod {

class HR_EXTERN ConfigurationBuilder
    : public Builder<Configuration>
{
  public:
    ConfigurationBuilder():
        m_connectionTimeout(60000),
        m_forceReturnValue(false),
        m_keySizeEstimate(64),
        m_pingOnStartup(true),
        m_protocolVersion(PROTOCOL_VERSION_12),
        m_socketTimeout(60000),
        m_tcpNoDelay(true),
        m_valueSizeEstimate(512),
        connectionPoolConfigurationBuilder(*this),
        sslConfigurationBuilder(*this)
      { };

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
    virtual Configuration create();
    virtual ConfigurationBuilder& read(Configuration& bean);
    static const char* PROTOCOL_VERSION_12;


  private:
    int m_connectionTimeout;
    bool m_forceReturnValue;
    int m_keySizeEstimate;
    bool m_pingOnStartup;
    std::string m_protocolVersion;
    std::vector<ServerConfigurationBuilder*> m_servers;
    int m_socketTimeout;
    bool m_tcpNoDelay;
    int m_valueSizeEstimate;

    ConnectionPoolConfigurationBuilder connectionPoolConfigurationBuilder;
    SslConfigurationBuilder sslConfigurationBuilder;
};

}} // namespace

#endif // ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
