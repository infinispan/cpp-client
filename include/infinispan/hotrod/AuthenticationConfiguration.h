/*
 * AuthenticationConfiguration.h
 *
 *  Created on: Jan 27, 2017
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_AUTHENTICATIONCONFIGURATION_H_
#define INCLUDE_INFINISPAN_HOTROD_AUTHENTICATIONCONFIGURATION_H_
#include <sasl/sasl.h>
namespace infinispan {
namespace hotrod {

/**
 * AuthenticationConfiguration object along with its factory AuthenticationConfigurationBuilder represent
 * used by ConfigurationBuilder for configuring RemoteCacheManager.
 *
 */

class AuthenticationConfiguration
{
public:
    AuthenticationConfiguration(std::vector<sasl_callback_t> callbackHandler, /*std::map<std::string, std::string> userTraits,*/ bool enabled, std::string saslMechanism, std::map<std::string, std::string> saslProperties, std::string serverName)
                      : enabled(enabled), callbackHandler(callbackHandler), /* userTraits(userTraits),*/ saslMechanism(saslMechanism),
                        saslProperties(saslProperties), serverName(serverName) {}
    AuthenticationConfiguration() : enabled(false) {}
    bool isEnabled() const { return enabled; }
private:
    bool enabled;
    std::vector<sasl_callback_t> callbackHandler;
//    std::map<std::string, std::string> clientSubject;
    std::string saslMechanism;
    std::map<std::string, std::string> saslProperties;
    std::string serverName;
};

}}

#endif /* INCLUDE_INFINISPAN_HOTROD_AUTHENTICATIONCONFIGURATION_H_ */
