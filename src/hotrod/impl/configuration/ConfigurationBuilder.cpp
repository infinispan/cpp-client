#include <sstream>
#include <fstream>
#include <iostream>

#include "infinispan/hotrod/ConfigurationBuilder.h"

namespace infinispan {
namespace hotrod {

const char* ConfigurationBuilder::PROTOCOL_VERSION_12 = "1.2";

ServerConfigurationBuilder& ConfigurationBuilder::addServer()
{
  ServerConfigurationBuilder* builder = new ServerConfigurationBuilder(*this);
  m_servers.push_back(builder);
  return *builder;
}

ConfigurationBuilder& ConfigurationBuilder::addServers(std::string servers)
{
  std::vector<std::string> splittedServers;
  std::istringstream originalStream(servers);
  std::string stringHelper;
  std::string hostHelper;
  std::string portHelper;
  int portInt;

  while(std::getline(originalStream,stringHelper,';')) {

      std::istringstream singleServerStream(stringHelper);
      std::getline(singleServerStream,hostHelper,':');
      std::getline(singleServerStream,portHelper,':');
      std::istringstream portStream(portHelper);
      portStream >> portInt;
      addServer().host(hostHelper).port(portInt);
  }

  return *this;
}

ConnectionPoolConfigurationBuilder& ConfigurationBuilder::connectionPool()
{
  return ConfigurationBuilder::connectionPoolConfigurationBuilder;
}

ConfigurationBuilder& ConfigurationBuilder::connectionTimeout(int connectionTimeout_)
{
  ConfigurationBuilder::m_connectionTimeout = connectionTimeout_;
  return *this;
}

ConfigurationBuilder& ConfigurationBuilder::forceReturnValues(bool forceReturnValues_)
{
  ConfigurationBuilder::m_forceReturnValue = forceReturnValues_;
  return *this;
}

ConfigurationBuilder& ConfigurationBuilder::keySizeEstimate(int keySizeEstimate_)
{
  ConfigurationBuilder::m_keySizeEstimate = keySizeEstimate_;
  return *this;
}

ConfigurationBuilder& ConfigurationBuilder::pingOnStartup(bool pingOnStartup_)
{
  ConfigurationBuilder::m_pingOnStartup = pingOnStartup_;
  return *this;
}

ConfigurationBuilder& ConfigurationBuilder::protocolVersion(std::string protocolVersion_)
{
  ConfigurationBuilder::m_protocolVersion = protocolVersion_;
  return *this;
}

ConfigurationBuilder& ConfigurationBuilder::socketTimeout(int socketTimeout_)
{
  ConfigurationBuilder::m_socketTimeout = socketTimeout_;
  return *this;
}

SslConfigurationBuilder& ConfigurationBuilder::ssl()
{
  return ConfigurationBuilder::sslConfigurationBuilder;
}

ConfigurationBuilder& ConfigurationBuilder::tcpNoDelay(bool tcpNoDelay_)
{
  ConfigurationBuilder::m_tcpNoDelay = tcpNoDelay_;
  return *this;
}

ConfigurationBuilder& ConfigurationBuilder::valueSizeEstimate(int valueSizeEstimate_)
{
  ConfigurationBuilder::m_valueSizeEstimate = valueSizeEstimate_;
  return *this;
}

Configuration ConfigurationBuilder::build()
{
  return ConfigurationBuilder::create();
}

Configuration ConfigurationBuilder::create()
{
  std::vector<ServerConfiguration> servers;
  if(m_servers.size() > 0) {
      for(std::vector<ServerConfigurationBuilder *>::iterator it = m_servers.begin(); it < m_servers.end(); it++) {
          servers.push_back((*it)->create());
      }
  } else {
      servers.push_back(ServerConfiguration("127.0.0.1", 11222));
  }

  return Configuration(m_protocolVersion,
      connectionPoolConfigurationBuilder.create(),
      m_connectionTimeout,
      m_forceReturnValue,
      m_keySizeEstimate,
      m_pingOnStartup,
      servers,
      m_socketTimeout,
      sslConfigurationBuilder.create(),
      m_tcpNoDelay,
      m_keySizeEstimate);
}

ConfigurationBuilder& ConfigurationBuilder::read(Configuration& bean)
{
  // FIXME: read pool, ssl and server configs
  m_protocolVersion = bean.getProtocolVersion();
  m_connectionTimeout = bean.getConnectionTimeout();
  m_forceReturnValue = bean.isForceReturnValue();
  m_pingOnStartup = bean.isPingOnStartup();
  m_socketTimeout = bean.getSocketTimeout();
  m_tcpNoDelay = bean.isTcpNoDelay();
  m_keySizeEstimate = bean.getKeySizeEstimate();
  m_valueSizeEstimate = bean.getValueSizeEstimate();

  return *this;
}


}} /* namespace */
