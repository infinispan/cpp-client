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
    AuthenticationConfiguration(std::vector<sasl_callback_t> callbackHandler, bool enabled, std::string saslMechanism, std::string serverFQDN)
                      : enabled(enabled), callbackHandler(callbackHandler), saslMechanism(saslMechanism), serverFQDN(serverFQDN) {}
    AuthenticationConfiguration() : enabled(false) {}
    bool isEnabled() const { return enabled; }

    const std::string& getSaslMechanism() const {
        return saslMechanism;
    }

    const std::vector<sasl_callback_t>& getCallbackHandler() const {
        return callbackHandler;
    }

    const std::string& getServerFqdn() const {
        return serverFQDN;
    }

private:
    bool enabled;
    std::vector<sasl_callback_t> callbackHandler;
    std::string saslMechanism;
    std::string serverFQDN;
};

}}

#endif /* INCLUDE_INFINISPAN_HOTROD_AUTHENTICATIONCONFIGURATION_H_ */
