#include "infinispan/hotrod/Configuration.h"

namespace infinispan {
namespace hotrod {

const char* Configuration::PROTOCOL_VERSION_12 = "1.2";

const std::string& Configuration::getProtocolVersion() const
{
  return protocolVersion;
}

}} /* namespace */
