/*
 * SecurityConfigurationBuilder.h
 *
 *  Created on: Jan 27, 2017
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_SECURITYCONFIGURATIONBUILDER_H_
#define INCLUDE_INFINISPAN_HOTROD_SECURITYCONFIGURATIONBUILDER_H_
#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/AuthenticationConfigurationBuilder.h"
#include "infinispan/hotrod/SslConfigurationBuilder.h"
namespace infinispan {
namespace hotrod {

class SecurityConfigurationBuilder : public ConfigurationChildBuilder
{
public:
    SecurityConfigurationBuilder(ConfigurationBuilder& _builder) : ConfigurationChildBuilder(_builder), sslConfigurationBuilder(_builder) {}
    SecurityConfiguration create() {
       return SecurityConfiguration(sslConfigurationBuilder.create(), authenticationConfigurationBuilder.create());
    }
    SslConfigurationBuilder& getSslConfigurationBuilder() { return sslConfigurationBuilder; }
    AuthenticationConfigurationBuilder& authentication() { return authenticationConfigurationBuilder; }
private:
    AuthenticationConfigurationBuilder authenticationConfigurationBuilder;
    SslConfigurationBuilder sslConfigurationBuilder;
};

}}



#endif /* INCLUDE_INFINISPAN_HOTROD_SECURITYCONFIGURATIONBUILDER_H_ */
