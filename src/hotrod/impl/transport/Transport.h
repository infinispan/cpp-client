#ifndef ISPN_HOTROD_TRANSPORT_TRANSPORT_H
#define ISPN_HOTROD_TRANSPORT_TRANSPORT_H



#include "infinispan/hotrod/types.h"
#include "hotrod/sys/types.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class Transport
{
  public:
    virtual void flush() = 0;
    virtual void writeByte(uint8_t uchar) = 0;
    virtual void writeVInt(uint32_t uint) = 0;
    virtual void writeVLong(uint64_t ulong) = 0;
    virtual void writeArray(const hrbytes& bytes) = 0;

    virtual void writeLong(int64_t slong) = 0;

    virtual uint8_t readByte() = 0;
    virtual uint32_t readVInt() = 0;
    virtual uint64_t readVLong() = 0;
    virtual hrbytes readArray() = 0;

    virtual int64_t readLong() = 0;
    virtual std::string readString() = 0;

    virtual void release() = 0;
    virtual void invalidate() = 0;

    virtual ~Transport() {}
};

}}} // namespace infinispan::hotrod::transport

#endif  // ISPN_HOTROD_TRANSPORT_TRANSPORT_H

