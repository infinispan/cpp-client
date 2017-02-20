#ifndef ISPN_HOTROD_SYS_SSLSOCKET_H
#define ISPN_HOTROD_SYS_SSLSOCKET_H

#include <hotrod/sys/Socket.h>
#include <openssl/bio.h>

namespace infinispan {
namespace hotrod {
namespace sys {

class SSLSocket: public infinispan::hotrod::sys::Socket
{
  public:
    static SSLSocket* create(const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile);

    SSLSocket(const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile);
    virtual ~SSLSocket();

    virtual void connect(const std::string& host, int port, int timeout);
    virtual void close();
    virtual void setTcpNoDelay(bool tcpNoDelay);
    virtual void setTimeout(int timeout);
    virtual size_t read(char *p, size_t n);
    virtual void write(const char *p, size_t n);
    virtual int getSocket();

  private:
    void logAndThrow(const std::string& host, const int port, const std::string& msg);

    Socket *m_socket;
    BIO *m_bio;
    SSL_CTX* m_ctx;
    SSL *m_ssl;
    std::string m_serverCAPath;
    std::string m_serverCAFile;
    std::string m_clientCertificateFile;
    std::string m_sniHostName;
    std::string host;
    int port;

    class OpenSSLInitializer
    {
      public:
        OpenSSLInitializer();
    };

    static OpenSSLInitializer initializer;

};

}}} // namespace

#endif  /* ISPN_HOTROD_SYS_SSLSOCKET_H */
