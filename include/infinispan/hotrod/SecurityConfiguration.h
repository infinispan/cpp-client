/*
 * SecurityConfiguration.h
 *
 *  Created on: Jan 27, 2017
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_SECURITYCONFIGURATION_H_
#define INCLUDE_INFINISPAN_HOTROD_SECURITYCONFIGURATION_H_

#include "infinispan/hotrod/SslConfiguration.h"
#include "infinispan/hotrod/AuthenticationConfiguration.h"
namespace infinispan {
namespace hotrod {

class SecurityConfiguration
{
public:
    //SecurityConfiguration() {}
    SecurityConfiguration(const SslConfiguration& sslConfiguration) : sslConfiguration(sslConfiguration) {}
    SecurityConfiguration(const SslConfiguration sslConfiguration, const AuthenticationConfiguration autheticationConfiguration) :
                    sslConfiguration(sslConfiguration), authenticationConfiguration(autheticationConfiguration) {}
    const SslConfiguration& getSslConfiguration() const { return sslConfiguration; }
    const AuthenticationConfiguration& getAuthenticationConfiguration() const { return authenticationConfiguration; }
private:
    SslConfiguration sslConfiguration;
    AuthenticationConfiguration authenticationConfiguration;
};
}}


#endif /* INCLUDE_INFINISPAN_HOTROD_SECURITYCONFIGURATION_H_ */
