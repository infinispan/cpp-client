/*
 * ServerConfigurationBuilder.cpp
 *
 *  Created on: Jul 18, 2013
 *      Author: samuele
 */

#include "hotrod/impl/configuration/ServerConfigurationBuilder.h"

namespace infinispan {
namespace hotrod {


ServerConfigurationBuilder& ServerConfigurationBuilder::host(std::string hostParam)
{
  ServerConfigurationBuilder::internalHost = hostParam;
  return *this;
}

ServerConfigurationBuilder& ServerConfigurationBuilder::port(int portParam)
{
  ServerConfigurationBuilder::internalPort = portParam;
  return *this;
}

ServerConfiguration ServerConfigurationBuilder::create()
{
  return ServerConfiguration(ServerConfigurationBuilder::internalHost, ServerConfigurationBuilder::internalPort);
}

ServerConfigurationBuilder& ServerConfigurationBuilder::read(ServerConfiguration& bean)
{
  ServerConfigurationBuilder::internalHost = bean.getHost();
  ServerConfigurationBuilder::internalPort = bean.getPort();
  return *this;

}

}} //namespace
