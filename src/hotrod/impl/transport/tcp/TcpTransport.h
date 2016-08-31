#ifndef ISPN_HOTROD_TCPTRANSPORT_H
#define ISPN_HOTROD_TCPTRANSPORT_H



#include <infinispan/hotrod/InetSocketAddress.h>
#include <sstream>

#include "infinispan/hotrod/defs.h"
#include "hotrod/impl/transport/AbstractTransport.h"
#include "hotrod/impl/transport/tcp/Socket.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class TransportFactory;

class TcpTransport : public AbstractTransport
{
  public:
    TcpTransport(const InetSocketAddress& address, TransportFactory& factory);

    void flush();
    void writeByte(uint8_t uchar);
    void writeVInt(uint32_t uint);
    void writeVLong(uint64_t ulong);
    void writeBytes(const std::vector<char>& bytes);

    uint8_t readByte();
    uint32_t readVInt();
    uint64_t readVLong();
    std::vector<char> readBytes(uint32_t size);

    void release();
    void invalidate();
    void destroy();
    virtual bool targets(const InetSocketAddress& arg) const
    {
    	return arg==getServerAddress();
    }
    const InetSocketAddress& getServerAddress() const;

  protected:
    TcpTransport(const InetSocketAddress& address, TransportFactory& factory, sys::Socket *sock);
    //Testing only!
    TcpTransport();
    Socket socket;
    bool invalid;
    InetSocketAddress* serverAddress;
    bool isValid();

  friend class TcpTransportFactory;
};

}}} // namespace infinispan::hotrod::transport::tcp

#endif  /* ISPN_HOTROD_TCPTRANSPORT_H */

