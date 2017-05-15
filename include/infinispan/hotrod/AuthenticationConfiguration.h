/*
 * AuthenticationConfiguration.h
 *
 *  Created on: Jan 27, 2017
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_AUTHENTICATIONCONFIGURATION_H_
#define INCLUDE_INFINISPAN_HOTROD_AUTHENTICATIONCONFIGURATION_H_
namespace infinispan {
namespace hotrod {

#if !defined _WIN32 && !defined _WIN64
#include "sasl/sasl.h"

#else

typedef struct sasl_callback {
    /* Identifies the type of the callback function.
     * Mechanisms must ignore callbacks with id's they don't recognize.
     */
    unsigned long id;
    int (*proc)(void);
    void *context;
} sasl_callback_t;


#endif

typedef int (*hr_sasl_callback_ft)(void);

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
