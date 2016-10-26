#ifndef SSLCONFIGURATIONBUILDER_H_
#define SSLCONFIGURATIONBUILDER_H_

#include "infinispan/hotrod/ImportExport.h"
#include "ConfigurationChildBuilder.h"
 

namespace infinispan {
namespace hotrod {


class SslConfigurationBuilder
  : public ConfigurationChildBuilder
{
  public:
    SslConfigurationBuilder(ConfigurationBuilder &parent): ConfigurationChildBuilder(parent),
        m_enabled(false), m_serverCAPath(), m_serverCAFile(), m_clientCertificateFile(), m_sniHostName() {}

    SslConfiguration create() {
        return SslConfiguration(m_enabled, m_serverCAPath, m_serverCAFile, m_clientCertificateFile, m_sniHostName);
    }
    virtual SslConfigurationBuilder& read(SslConfiguration& configuration) {
        m_enabled = configuration.enabled();
        m_serverCAPath = configuration.serverCAPath();
        m_serverCAFile = configuration.serverCAFile();
        m_clientCertificateFile = configuration.clientCertificateFile();
        m_sniHostName = configuration.sniHostName();
        return *this;
    }
    virtual void validate() {};
    
    /***
     * Enables SSL support
     *
     * \return ServerConfigurationBuilder for further configuration
     */
    SslConfigurationBuilder& enable()
    {
        m_enabled = true;
        return *this;
    }
    
    /***
     * Disables SSL support
     *
     * \return ServerConfigurationBuilder for further configuration
     */
    SslConfigurationBuilder& disable()
    {
        m_enabled = false;
        return *this;
    }
    
    /***
     * Enables SSL support
     *
     * \return ServerConfigurationBuilder for further configuration
     */
    SslConfigurationBuilder& enabled(bool enabled)
    {
        m_enabled = enabled;
        return *this;
    }

    /**
     * Specifies the path where CA certificates are stored in PEM format
     *
     * \return ServerConfigurationBuilder for further configuration
     */
    SslConfigurationBuilder& serverCAPath(const std::string& _CAPath)
    {
        m_serverCAPath = _CAPath;
        return *this;
    }

    /**
     * Specifies a single CA certificate in PEM format
     *
     * \return ServerConfigurationBuilder for further configuration
     */
    SslConfigurationBuilder& serverCAFile(const std::string& _CAFile)
    {
        m_serverCAFile = _CAFile;
        return *this;
    }

    /**
     * Specifies the client certificate to use. The certificate should be in PEM format.
     *
     * \return ServerConfigurationBuilder for further configuration
     */
    SslConfigurationBuilder& clientCertificateFile(const std::string& _clientCertificateFile)
    {
        m_clientCertificateFile = _clientCertificateFile;
        return *this;
    }

    /**
     * Specifies the hostname for SNI.
     *
     * \return ServerConfigurationBuilder for further configuration
     */
    SslConfigurationBuilder& sniHostName(const std::string& _sniHostName)
    {
        m_sniHostName = _sniHostName;
        return *this;
    }

    /**
     * Returns the certificate file used for the client
     *
     * \return the file which contains the client certificate
     */
    const std::string& getSniHostName() const
    {
        return m_sniHostName;
    }


private:
    bool m_enabled;
    std::string m_serverCAPath;
    std::string m_serverCAFile;
    std::string m_clientCertificateFile;
    std::string m_sniHostName;
};

}}

#endif /* SSLCONFIGURATIONBUILDER_H_ */
