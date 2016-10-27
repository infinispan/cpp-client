#include "infinispan/hotrod/exceptions.h"
#include "SSLSocket.h"
#include "hotrod/sys/Log.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
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

    const SSL_METHOD* method = TLSv1_2_client_method();
    m_ctx = SSL_CTX_new(method);
    if (!m_ctx) {
        logAndThrow(host, port, "SSL_CTX_new");
    }
    SSL_CTX_set_verify(m_ctx, SSL_VERIFY_PEER, 0);
    if (!m_clientCertificateFile.empty()) {
        DEBUG("Using user-supplied client certificate %s", m_clientCertificateFile.c_str());
        if (!SSL_CTX_use_certificate_file(m_ctx, m_clientCertificateFile.c_str(), SSL_FILETYPE_PEM)) {
            logAndThrow(host, port, "SSL_CTX_use_certificate_file");
        }
    }
    SSL_CTX_set_options(m_ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
    if (m_serverCAFile.empty() &&  m_serverCAPath.empty()) {
        if(!SSL_CTX_set_default_verify_paths(m_ctx)) {
            logAndThrow(host, port, "SSL_CTX_set_default_verify_paths");
        }
    }
    if (!SSL_CTX_load_verify_locations(m_ctx, m_serverCAFile.empty() ? 0 : m_serverCAFile.c_str(), m_serverCAPath.empty() ? 0 : m_serverCAPath.c_str())) {
        logAndThrow(host, port, "SSL_CTX_load_verify_locations");
    }

    m_ssl = SSL_new(m_ctx);
    if (!m_ssl) {
        logAndThrow(host, port, "SSL_new");
    }
    const char* const PREFERRED_CIPHERS = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
    if (!SSL_set_cipher_list(m_ssl, PREFERRED_CIPHERS)) {
        logAndThrow(host, port, "SSL_set_cipher_list");
    }
    m_bio = BIO_new_socket(getSocket(), BIO_NOCLOSE);
    if (!m_bio) {
        logAndThrow(host, port, "BIO_new_socket");
    }

    SSL_set_bio(m_ssl, m_bio, m_bio);

    if (!m_sniHostName.empty())
    {
        if (!SSL_set_tlsext_host_name(m_ssl, &m_sniHostName[0])) {
            logAndThrow(host,port, "SSL_set_tlsext_host_name");
        }
    }

    if (!SSL_connect(m_ssl)) {
        logAndThrow(host, port, "SSL_connect");
    }
    X509* cert = SSL_get_peer_certificate(m_ssl);
    if (cert) {
        DEBUG("X509 Certificate %s", cert->name);
        X509_free(cert);
    } else {
        logAndThrow(host, port, "SSL_get_peer_certificate");
    }
    /*if (SSL_get_verify_result(m_ssl) != X509_V_OK) {
        logAndThrow(host, port, "SSL_get_verify_result");
    }*/
}

size_t SSLSocket::read(char *p, size_t n) {
    return SSL_read(m_ssl, p, n);
}

void SSLSocket::write(const char *p, size_t n) {
    SSL_write(m_ssl, p, n);
    if (ERR_peek_error())
        logAndThrow("", 0, "BIO_write");
}

void SSLSocket::close() {
    if(m_bio != 0)
        BIO_free_all(m_bio);
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
