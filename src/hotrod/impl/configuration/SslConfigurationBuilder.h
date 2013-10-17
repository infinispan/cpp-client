#ifndef SSLCONFIGURATIONBUILDER_H_
#define SSLCONFIGURATIONBUILDER_H_

#include "infinispan/hotrod/ImportExport.h"
#include "Builder.h"
#include "ConfigurationChildBuilder.h"


namespace infinispan {
namespace hotrod {


class HR_EXTERN SslConfigurationBuilder
  : public infinispan::hotrod::Builder<SslConfiguration>, public ConfigurationChildBuilder
{
  public:
    SslConfigurationBuilder(ConfigurationBuilder& builder);
    virtual SslConfiguration create();
    virtual SslConfigurationBuilder& read(SslConfiguration& bean);
};

}}

#endif /* SSLCONFIGURATIONBUILDER_H_ */
