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
    SecurityConfigurationBuilder(ConfigurationBuilder& _builder) : ConfigurationChildBuilder(_builder), ssl(_builder) {}
    SecurityConfiguration create() {
       return SecurityConfiguration(ssl.create(), authentication.create());
    }
    SslConfigurationBuilder& getSslConfigurationBuilder() { return ssl; }
    AuthenticationConfigurationBuilder& getAuthConfigurationBuilder() { return authentication; }
private:
    AuthenticationConfigurationBuilder authentication;
    SslConfigurationBuilder ssl;
};

}}



#endif /* INCLUDE_INFINISPAN_HOTROD_SECURITYCONFIGURATIONBUILDER_H_ */
