#include "infinispan/hotrod/exceptions.h"
#include "SSLSocket.h"
#include "hotrod/sys/Log.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace infinispan::hotrod::sys;

SSLSocket::OpenSSLInitializer::OpenSSLInitializer() {
    SSL_load_error_strings();
    SSL_library_init();
}

SSLSocket::OpenSSLInitializer SSLSocket::initializer;

SSLSocket::SSLSocket(const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile, const std::string& _sniHostName):
    m_socket(Socket::create()), m_bio(0), m_ctx(0), m_ssl(0),
    m_serverCAPath(_serverCAPath), m_serverCAFile(_serverCAFile), m_clientCertificateFile(_clientCertificateFile), m_sniHostName(_sniHostName) {}

SSLSocket::~SSLSocket()
{
    delete m_socket;
}

void SSLSocket::connect(const std::string& host, int port, int timeout) {
    m_socket->connect(host, port, timeout);
    this->host = host;
    this->port = port;
	#if (OPENSSL_VERSION_NUMBER <= 0x10100000L)
    	const SSL_METHOD* method = SSLv23_client_method();
	#else
    	const SSL_METHOD* method = TLS_client_method();
    #endif
    m_ctx = SSL_CTX_new(method);
    if (!m_ctx) {
        logAndThrow(host, port, "SSL_CTX_new");
    }
    SSL_CTX_set_verify(m_ctx, SSL_VERIFY_PEER, 0);
    SSL_CTX_set_options(m_ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
    if (m_serverCAFile.empty() &&  m_serverCAPath.empty()) {
        if(!SSL_CTX_set_default_verify_paths(m_ctx)) {
            if(m_ctx != 0)
                SSL_CTX_free(m_ctx);
            m_socket->close();
            logAndThrow(host, port, "SSL_CTX_set_default_verify_paths");
        }
    }
    if (!SSL_CTX_load_verify_locations(m_ctx, m_serverCAFile.empty() ? 0 : m_serverCAFile.c_str(), m_serverCAPath.empty() ? 0 : m_serverCAPath.c_str())) {
        if(m_ctx != 0)
            SSL_CTX_free(m_ctx);
        m_socket->close();
        logAndThrow(host, port, "SSL_CTX_load_verify_locations");
    }

    if (!m_clientCertificateFile.empty()) {
        DEBUG("Using user-supplied client certificate %s", m_clientCertificateFile.c_str());
        if (!SSL_CTX_use_certificate_file(m_ctx, m_clientCertificateFile.c_str(), SSL_FILETYPE_PEM)) {
            logAndThrow(host, port, "SSL_CTX_use_certificate_file");
        }
        if (!SSL_CTX_use_PrivateKey_file(m_ctx, m_clientCertificateFile.c_str(), SSL_FILETYPE_PEM)) {
            logAndThrow(host, port, "SSL_CTX_use_certificate_file");
        }
    }

    m_ssl = SSL_new(m_ctx);
    if (!m_ssl) {
        if(m_ctx != 0)
            SSL_CTX_free(m_ctx);
        m_socket->close();
        logAndThrow(host, port, "SSL_new");
    }
    const char* const PREFERRED_CIPHERS = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
    if (!SSL_set_cipher_list(m_ssl, PREFERRED_CIPHERS)) {
        close();
        logAndThrow(host, port, "SSL_set_cipher_list");
    }
    //const char* const PREFERRED_CIPHERS_TLSv1_3 = "TLSv1.3";
    //if (!SSL_set_ciphersuites(m_ssl, PREFERRED_CIPHERS_TLSv1_3)) {
    //    close();
    //    logAndThrow(host, port, "SSL_set_ciphersuite");
    //}
    m_bio = BIO_new_socket(getSocket(), BIO_NOCLOSE);
    if (!m_bio) {
        close();
        logAndThrow(host, port, "BIO_new_socket");
    }

    SSL_set_bio(m_ssl, m_bio, m_bio);

    if (!m_sniHostName.empty())
    {
        if (!SSL_set_tlsext_host_name(m_ssl, &m_sniHostName[0])) {
            close();
            logAndThrow(host,port, "SSL_set_tlsext_host_name");
        }
    }

    if (!SSL_connect(m_ssl)) {
        logAndThrow(host, port, "SSL_connect");
    }
    X509* cert = SSL_get_peer_certificate(m_ssl);
    if (cert) {
    #if (OPENSSL_VERSION_NUMBER >= 0x10100000L)
	#define MAX_SIZE 1000
    	char* line = new char[ MAX_SIZE + 1 ];
    	X509_NAME_oneline( X509_get_subject_name( cert ), line, MAX_SIZE);
    	DEBUG("X509 Certificate %s", line);
    	delete[] line;
#else
        DEBUG("X509 Certificate %s", cert->name);
#endif
        X509_free(cert);
    } else {
        close();
        logAndThrow(host, port, "SSL_get_peer_certificate");
    }
}

size_t SSLSocket::read(char *p, size_t n) {
    int ret= SSL_read(m_ssl, p, n);
    if (ret > 0)
        return ret;
    int errcode = SSL_get_error(m_ssl, ret);
    std::string errMsg;
    if (ret < 0)
    {
    	errMsg = "SSL: Read error. Error code: ";
    }
    else
    {
        errMsg= ( errcode==SSL_ERROR_ZERO_RETURN)
        		? "SSL: No read. Shutdown complete."
        		: "SSL: No read. Shutdown incomplete.";
    }
    errMsg+=" (host: "+host+" port: ("+std::to_string(port)+")";
    if (errcode != 0) {
        char buf[256];
        strerror_r(errcode, buf, 256);
        errMsg+" "+buf;
    }
    throw infinispan::hotrod::TransportException(host, port, errMsg, errcode);
}

void SSLSocket::write(const char *p, size_t n) {
    SSL_write(m_ssl, p, n);
    if (ERR_peek_error())
        logAndThrow("", 0, "BIO_write");
}

void SSLSocket::close() {
    SSL_shutdown(m_ssl);
    if(m_ssl != 0)
        SSL_free(m_ssl);
    if(m_ctx != 0)
        SSL_CTX_free(m_ctx);
    m_socket->close();
}

void SSLSocket::setTcpNoDelay(bool tcpNoDelay) {
    m_socket->setTcpNoDelay(tcpNoDelay);
}

void SSLSocket::setTimeout(int timeout) {
    m_socket->setTimeout(timeout);
}

int SSLSocket::getSocket() {
    return m_socket->getSocket();
}

void SSLSocket::logAndThrow(const std::string& host, const int port, const std::string& msg) {
    int i;
    DEBUG(msg.c_str());
    while((i=ERR_get_error())){
        char buf[1024];
        ERR_error_string_n(i,buf,sizeof(buf));
        ERROR("Error %d: %s", i, buf);
    }
    throw TransportException(host, port, msg, -1);
}

SSLSocket* SSLSocket::create(const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile, const std::string& _sniHostName) {
    return new SSLSocket(_serverCAPath, _serverCAFile, _clientCertificateFile, _sniHostName);
}
