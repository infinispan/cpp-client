/*
 * ServerConfiguration.cpp
 *
 *  Created on: Jul 11, 2013
 *      Author: samuele
 */
#include "hotrod/impl/configuration/ServerConfiguration.h"

namespace infinispan {
namespace hotrod {

ServerConfiguration::ServerConfiguration(std::string host,
        int port)
{
  ServerConfiguration::host = host;
  ServerConfiguration::port = port;
}

const std::string& ServerConfiguration::getHost() const
{
  return ServerConfiguration::host;
}

const int& ServerConfiguration::getPort() const
{
  return ServerConfiguration::port;
}



}
}
