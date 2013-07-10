#include "hotrod/configuration/ConfigurationBuilder.h"

namespace infinispan {
namespace hotrod {


namespace configuration {

const char* ConfigurationBuilder::PROTOCOL_VERSION_12 = "1.2";

Configuration ConfigurationBuilder::build()
{
  Configuration* configuration = new infinispan::hotrod::Configuration();
  return *configuration;
}

}}} /* namespace */
