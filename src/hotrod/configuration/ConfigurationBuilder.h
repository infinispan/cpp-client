#ifndef ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
#define ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H

#include "infinispan/hotrod/Configuration.h"

namespace infinispan {
namespace hotrod {
namespace configuration {

// TODO
class ConfigurationBuilder
{
  public:
    Configuration build();
    static const char* PROTOCOL_VERSION_12;
};

}}} // namespace

#endif // ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
