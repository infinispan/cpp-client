#ifndef ISPN_HOTROD_CONFIGURATION_H
#define ISPN_HOTROD_CONFIGURATION_H

#include <string>

namespace infinispan {
namespace hotrod {

// TODO
class Configuration
{
  public:
    // TODO: spostare (java ConfigurationProperties)
    static const char* PROTOCOL_VERSION_12;

    const std::string & getProtocolVersion() const;

  private:
    std::string protocolVersion;
};

}} // namespace

#endif /* ISPN_HOTROD_CONFIGURATION_H */
