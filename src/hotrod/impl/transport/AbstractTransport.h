#ifndef ISPN_HOTROD_TRANSPORT_ABSTRACTTRANSPORT_H
#define ISPN_HOTROD_TRANSPORT_ABSTRACTTRANSPORT_H



#include <sstream>

#include "hotrod/impl/transport/Transport.h"

#include "hotrod/sys/Socket.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class TransportFactory;

class AbstractTransport : public Transport
{
  public:
	AbstractTransport(TransportFactory& transportFactory);

    void writeArray(const std::vector<char>& bytes);
    void writeLong(int64_t longValue);
    std::vector<char> readArray();
    int64_t readLong();
    int16_t readUnsignedShort();
    int32_t read4ByteInt();
    std::string readString();
    TransportFactory& getTransportFactory();

  protected:
    virtual void writeBytes(const std::vector<char>& bytes) = 0;
    virtual std::vector<char> readBytes(uint32_t size) = 0;

  private:
    TransportFactory& transportFactory;
};

}}} // namespace infinispan::hotrod::transport

#endif  // ISPN_HOTROD_TRANSPORT_ABSTRACTTRANSPORT_H

