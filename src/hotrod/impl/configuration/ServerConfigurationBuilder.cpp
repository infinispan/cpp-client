#include "hotrod/impl/configuration/ServerConfigurationBuilder.h"

namespace infinispan {
namespace hotrod {

ServerConfigurationBuilder::ServerConfigurationBuilder(ConfigurationBuilder& builder_) : ConfigurationChildBuilder(builder_), m_host("localhost"), m_port(11222) {
}

ServerConfigurationBuilder& ServerConfigurationBuilder::host(std::string host_)
{
  m_host = host_;
  return *this;
}

ServerConfigurationBuilder& ServerConfigurationBuilder::port(int port_)
{
  m_port = port_;
  return *this;
}

ServerConfiguration ServerConfigurationBuilder::create()
{
  return ServerConfiguration(m_host, m_port);
}

ServerConfigurationBuilder& ServerConfigurationBuilder::read(ServerConfiguration& bean)
{
  m_host = bean.getHost();
  m_port = bean.getPort();
  return *this;

}

}} //namespace
