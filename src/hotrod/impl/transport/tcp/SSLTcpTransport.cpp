#include <hotrod/impl/transport/tcp/SSLTcpTransport.h>
#include "infinispan/hotrod/exceptions.h"
#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"
#include "hotrod/sys/SSLSocket.h"

#include <iostream>
#include <cstring>
#include <istream>
#include <sstream>


namespace infinispan {
namespace hotrod {
namespace transport {

SSLTcpTransport::SSLTcpTransport(
    const InetSocketAddress& a, TransportFactory& factory, const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile)
: TcpTransport(a, factory, sys::SSLSocket::create(_serverCAPath, _serverCAFile, _clientCertificateFile)) {
}

//Testing purpose only!
SSLTcpTransport::SSLTcpTransport()
: TcpTransport(InetSocketAddress(), *(TransportFactory*) 0, sys::Socket::create()) {}
}}}
