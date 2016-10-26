#ifndef ISPN_HOTROD_SSLTCPTRANSPORT_H
#define ISPN_HOTROD_SSLTCPTRANSPORT_H

#include <infinispan/hotrod/InetSocketAddress.h>
#include <sstream>

#include "infinispan/hotrod/defs.h"
#include "hotrod/impl/transport/AbstractTransport.h"
#include "hotrod/impl/transport/tcp/Socket.h"
#include "TcpTransport.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class TransportFactory;

class SSLTcpTransport : public TcpTransport
{
  public:
    SSLTcpTransport(const InetSocketAddress& address, TransportFactory& factory, const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile, const std::string& _sniHostName);

  protected:
    //Testing only!
    SSLTcpTransport();

  friend class TcpTransportFactory;
};

}}} // namespace infinispan::hotrod::transport::tcp

#endif  /* ISPN_HOTROD_SSLTCPTRANSPORT_H */

