#ifndef ISPN_HOTROD_TRANSPORT_TRANSPORT_H
#define ISPN_HOTROD_TRANSPORT_TRANSPORT_H

#include <cstdint>
#include <vector>

namespace infinispan {
namespace hotrod {
namespace transport {

class TransportFactory;
class InetSocketAddress;

class Transport
{
  public:
    virtual void flush() = 0;
    virtual void writeByte(uint8_t uchar) = 0;
    virtual void writeVInt(uint32_t uint) = 0;
    virtual void writeVLong(uint64_t ulong) = 0;
    virtual void writeArray(const std::vector<char>& bytes) = 0;

    virtual void writeLong(int64_t slong) = 0;

    virtual uint8_t readByte() = 0;
    virtual uint32_t readVInt() = 0;
    virtual uint64_t readVLong() = 0;
    virtual std::vector<char> readArray() = 0;

    virtual int64_t readLong() = 0;
    virtual int16_t readUnsignedShort() = 0;
    virtual int32_t read4ByteInt() = 0;
    virtual std::string readString() = 0;

    virtual void release() = 0;
    virtual void invalidate() = 0;

    virtual TransportFactory& getTransportFactory() = 0;
    virtual bool targets(const InetSocketAddress&) const = 0;

    virtual ~Transport() {}
};

}}} // namespace infinispan::hotrod::transport

#endif  // ISPN_HOTROD_TRANSPORT_TRANSPORT_H
