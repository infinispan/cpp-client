#ifndef SSLCONFIGURATIONBUILDER_H_
#define SSLCONFIGURATIONBUILDER_H_

#include "infinispan/hotrod/ImportExport.h"
#include "Builder.h"
#include "ConfigurationChildBuilder.h"


namespace infinispan {
namespace hotrod {


class SslConfigurationBuilder
  : public infinispan::hotrod::Builder<SslConfiguration>, public ConfigurationChildBuilder
{
  public:
    SslConfigurationBuilder(ConfigurationBuilder& builder): ConfigurationChildBuilder(builder) {}
    virtual SslConfiguration create() {
        return SslConfiguration();
    }
    virtual SslConfigurationBuilder& read(SslConfiguration& bean) {
        (void) bean;
        return *this;
    }
};

}}

#endif /* SSLCONFIGURATIONBUILDER_H_ */
