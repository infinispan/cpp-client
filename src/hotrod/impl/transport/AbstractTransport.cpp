

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

void AbstractTransport::writeArray(const std::vector<char>& bytes)
{
  std::vector<char>& not_const_bytes = const_cast<std::vector<char>&>(bytes);
  writeVInt((uint32_t) not_const_bytes.size());
  writeBytes(bytes);
}

void AbstractTransport::writeString(const std::string& str)
{
    writeBytes(str.data(), str.size());
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
  std::vector<char> bytes(ptr,ptr+8);
  writeBytes(bytes);
}

std::vector<char> AbstractTransport::readArray()
{
  uint32_t size = readVInt();
  std::vector<char> result(readBytes(size));
  return result;
}

int64_t AbstractTransport::readLong()
{
  std::vector<char> longBytes= readBytes(8);
  int64_t result = 0;
  for (int i = 0; i < 8 ; i++) {
    result <<= 8;
    result ^= (int64_t) *(longBytes.data() + i) & 0xFF;
  }
  return result;
}

int16_t AbstractTransport::readUnsignedShort()
{
  std::vector<char> shortBytes= readBytes(2);
  int16_t result = 0;

  for (int i = 0; i < 2 ; i++) {
    result <<= 8;
    result ^= (int16_t) *(shortBytes.data() + i) & 0xFF;
  }
  return result;
}

int32_t AbstractTransport::read4ByteInt()
{
  int32_t result = 0;

  for (int i = 0; i < 4 ; i++) {
      result=(result << 8) + readByte();
  }
  return result;
}

// TODO
std::string AbstractTransport::readString() {
	std::vector<char> result = readArray();
	return std::string(result.data(), result.size());
}

}}} // namespace infinispan::hotrod::transport
