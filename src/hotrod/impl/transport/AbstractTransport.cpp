

#include "hotrod/impl/transport/AbstractTransport.h"


namespace infinispan {
namespace hotrod {
namespace transport {

AbstractTransport::AbstractTransport(TransportFactory& tf) :
    transportFactory(tf)
{}

TransportFactory& AbstractTransport::getTransportFactory(){
    return transportFactory;
}

void AbstractTransport::writeArray(const hrbytes& bytes)
{
  hrbytes& not_const_bytes = const_cast<hrbytes&>(bytes);
  writeVInt((uint32_t) not_const_bytes.length());
  writeBytes(bytes);
}

void AbstractTransport::writeLong(int64_t longValue)
{
  /*
  for (char* ptr = bytes.bytes() + 8 ; ptr > bytes.bytes() ; --ptr) {
       // TODO: verificare operatore java >>>
       *ptr = (char) longValue >> ( (bytes.bytes() + 8 - ptr)* 8);
  }
  */
  char ptr[8];
  for (int i = 0 ; i < 8 ; i++) {
    ptr[7-i] = (char) ((longValue) >> (8*i));
  }
  hrbytes bytes(ptr,8);
  writeBytes(bytes);
}

hrbytes AbstractTransport::readArray()
{
  uint32_t size = readVInt();
  hrbytes result(readBytes(size));
  return result;
}

int64_t AbstractTransport::readLong()
{
  hrbytes longBytes= readBytes(8);
  int64_t result = 0;
  for (int i = 0; i < 8 ; i++) {
    result <<= 8;
    result ^= (int64_t) *(longBytes.bytes()+i) & 0xFF;
  }
  return result;
}

int16_t AbstractTransport::readUnsignedShort()
{
  hrbytes shortBytes= readBytes(2);
  int16_t result = 0;

  for (int i = 0; i < 2 ; i++) {
    result <<= 8;
    result ^= (int16_t) *(shortBytes.bytes()+i) & 0xFF;
  }
  return result;
}

int32_t AbstractTransport::read4ByteInt()
{
  hrbytes intBytes= readBytes(4);
  int32_t result = 0;

  for (int i = 0; i < 4 ; i++) {
      int shift = (4 - 1 - i) * 8;
      result += (*(intBytes.bytes()+i) & 0x000000FF) << shift;
  }
  return result;
}

// TODO
std::string AbstractTransport::readString() {
	hrbytes result = readArray();
	return std::string(result.bytes(),result.length());
}

}}} // namespace infinispan::hotrod::transport
