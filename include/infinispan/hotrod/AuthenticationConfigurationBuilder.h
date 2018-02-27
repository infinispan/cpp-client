/*
 * AuthenticationConfigurationBuilder.h
 *
 *  Created on: Jan 27, 2017
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_AUTHENTICATIONCONFIGURATIONBUILDER_H_
#define INCLUDE_INFINISPAN_HOTROD_AUTHENTICATIONCONFIGURATIONBUILDER_H_


namespace infinispan {
namespace hotrod {



class AuthenticationConfigurationBuilder
{
public:
    AuthenticationConfiguration create() {
       return AuthenticationConfiguration(m_callbackHandler, m_enabled, m_saslMechanism, m_serverFQDN);
    }
    /**
     * Specifies a set of callbacks to be used during the authentication handshake.
     */
    AuthenticationConfigurationBuilder& callbackHandler(std::vector<sasl_callback_t> &callbackHandler) {
       m_callbackHandler = callbackHandler;
       return *this;
    }

    /**
     * Configures whether authentication should be enabled or not
     * \param enabled boolean
     */
    AuthenticationConfigurationBuilder& enabled(bool enabled) {
       m_enabled = enabled;
       return *this;
    }

    /**
     * Enables authentication
     */
    AuthenticationConfigurationBuilder& enable() {
       m_enabled = true;
       return *this;
    }

    /**
     * Disables authentication
     */
    AuthenticationConfigurationBuilder& disable() {
       m_enabled = false;
       return *this;
    }

    /**
     * Selects the SASL mechanism to use for the connection to the server
     * \param saslMechanism the select mechanism: PLAIN, DIGEST-MD5, GSSAPI are available
     */
    AuthenticationConfigurationBuilder& saslMechanism(std::string saslMechanism) {
       m_saslMechanism = saslMechanism;
       return *this;
    }

    /**
     * Sets the name of the server as expected by the SASL protocol
     */
    AuthenticationConfigurationBuilder& serverFQDN(std::string serverFQDN) {
       m_serverFQDN = serverFQDN;
       return *this;
    }

private:
    bool m_enabled=false;
    std::vector<sasl_callback_t> m_callbackHandler;
    std::string m_saslMechanism;
    std::string m_serverFQDN;
};

}}


#endif /* INCLUDE_INFINISPAN_HOTROD_AUTHENTICATIONCONFIGURATIONBUILDER_H_ */
