#ifndef SSLCONFIGURATION_H_
#define SSLCONFIGURATION_H_

#include "infinispan/hotrod/ImportExport.h"

namespace infinispan {
namespace hotrod {


class HR_EXTERN SslConfiguration
{
  public:
    SslConfiguration(const bool _enabled, const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile, const std::string& _sniHostName):
        m_enabled(_enabled), m_serverCAPath(_serverCAPath), m_serverCAFile(_serverCAFile), m_clientCertificateFile(_clientCertificateFile), m_sniHostName(_sniHostName) {};
    SslConfiguration(const SslConfiguration& conf)
    {
        m_enabled=conf.m_enabled;
        m_serverCAPath=conf.m_serverCAPath;
        m_serverCAFile=conf.m_serverCAFile;
        m_clientCertificateFile=conf.m_clientCertificateFile;
        m_sniHostName=conf.m_sniHostName;
    }
    /**
     * Returns whether SSL is enabled
     *
     * \return true if enabled, false otherwise
     */
    const bool& enabled() const
    {
        return m_enabled;
    }

    /**
     * Returns the certificate path for server validation
     *
     * \return the path which contains CA certificates in PEM format to use for server verification
     */
    const std::string& serverCAPath() const
    {
        return m_serverCAPath;
    }

    /**
     * Returns the certificate file for server validation
     *
     * \return the file which contains the CA certificate to use for server verification
     */
    const std::string& serverCAFile() const
    {
        return m_serverCAFile;
    }

    /**
     * Returns the certificate file used for the client
     *
     * \return the file which contains the client certificate
     */
    const std::string& clientCertificateFile() const
    {
        return m_clientCertificateFile;
    }

    /**
     * Returns the certificate file used for the client
     *
     * \return the file which contains the client certificate
     */
    const std::string& sniHostName() const
    {
        return m_sniHostName;
    }

  private:
    bool m_enabled;
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif
    std::string m_serverCAPath;
    std::string m_serverCAFile;
    std::string m_clientCertificateFile;
    std::string m_sniHostName;
#if _MSC_VER
#pragma warning(pop)
#endif
};

}}

#endif /* SSLCONFIGURATION_H_ */
