#include <hotrod/impl/transport/tcp/SChannelTcpTransport.h>
#include "infinispan/hotrod/exceptions.h"
#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"
#include "hotrod/sys/SChannelSocket.h"

#include <iostream>
#include <cstring>
#include <istream>
#include <sstream>


namespace infinispan {
	namespace hotrod {
		namespace transport {

			SChannelTcpTransport::SChannelTcpTransport(
				const InetSocketAddress& a, TransportFactory& factory, const std::string& _serverCAPath, const std::string& _serverCAFile, const std::string& _clientCertificateFile)
				: TcpTransport(a, factory, sys::SChannelSocket::create(_serverCAPath, _serverCAFile, _clientCertificateFile)) {
			}

			//Testing purpose only!
			SChannelTcpTransport::SChannelTcpTransport()
				: TcpTransport(InetSocketAddress(), *(TransportFactory*)0, sys::Socket::create()) {}
		}
	}
}