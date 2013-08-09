#ifndef ISPN_HOTROD_TCPTRANSPORT_H
#define ISPN_HOTROD_TCPTRANSPORT_H



#include <sstream>

#include "hotrod/impl/transport/AbstractTransport.h"

#include "hotrod/sys/types.h"
#include "hotrod/impl/transport/tcp/Socket.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"

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
    void writeBytes(const hrbytes& bytes);

    uint8_t readByte();
    uint32_t readVInt();
    uint64_t readVLong();
    void readBytes(hrbytes& bytes, uint32_t size);

    void release();
    void invalidate();
    void destroy();

    const InetSocketAddress& getServerAddress();

  private:
    TcpTransport();
    Socket socket;
    bool invalid;
    const InetSocketAddress& serverAddress;

    bool isValid();

  friend class TcpTransportFactory;
};

}}} // namespace infinispan::hotrod::transport::tcp

#endif  /* ISPN_HOTROD_TCPTRANSPORT_H */

