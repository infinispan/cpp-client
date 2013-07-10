#ifndef ISPN_HOTROD_TCPTRANSPORT_H
#define ISPN_HOTROD_TCPTRANSPORT_H

#include <sstream>

#include "hotrod/impl/transport/AbstractTransport.h"

#include "hotrod/sys/types.h"
#include "hotrod/sys/Socket.h"
#include "hotrod/impl/transport/tcp/InputStream.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class TcpTransport : public AbstractTransport
{
  public:
    void flush();
    void writeByte(uint8_t uchar);
    void writeVInt(uint32_t uint);
    void writeVLong(uint64_t ulong);
    void writeBytes(const hrbytes& bytes);

    uint8_t readByte();
    uint32_t readVInt();
    uint64_t readVLong();
    void readBytes(hrbytes& bytes, uint32_t size);

  private:
    TcpTransport();
    HR_SHARED_PTR<sys::Socket> socket;
    std::ostringstream out;
    //HR_SHARED_PTR<char> in;
    //char * in;
    InputStream inStr;

  friend class TcpTransportFactory;
};

}}} // namespace infinispan::hotrod::transport::tcp

#endif  /* ISPN_HOTROD_TCPTRANSPORT_H */

