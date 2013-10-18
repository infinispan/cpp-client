#include "infinispan/hotrod/ServerConfiguration.h"

namespace infinispan {
namespace hotrod {

ServerConfiguration::ServerConfiguration(std::string hostPar,
        int portPar) : host(hostPar), port(portPar)
{

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
