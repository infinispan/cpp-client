#include "hotrod/impl/transport/tcp/InputStream.h"
#include "hotrod/sys/Socket.h"

#include <iostream>
#include <cstring>
#include <istream>
#include <sstream>


namespace infinispan {
namespace hotrod {

using sys::Socket;

namespace transport {

// TODO

InputStream::InputStream(sys::Socket& s) : socket(s), ptr(&buffer[0]), hasMore(true), capacity(0)
{}

void InputStream::read(char* buf, size_t size)
{

char* tmp_buffer = buf;
  while (capacity < size) {
    if (capacity != 0) {
      memcpy(tmp_buffer, ptr, capacity);
      tmp_buffer += capacity;
      size -= capacity;
    }
    if (!hasMore)
      throw HotRodClientException("stream not complete");
    capacity = socket.read(&buffer[0],BufferSize);
    ptr = &buffer[0];
    hasMore = capacity < BufferSize ? false : true;
  }
  memcpy(tmp_buffer, ptr, size);
  ptr += size;
  capacity -= size;
}

InputStream& operator>>(InputStream& in, char& c)
{
  in.read(&c,1);
  return in;
}

char InputStream::read()
{
  char c;
  read(&c,1);
  return c;
}

}}} // namespace infinispan::hotrod::transport::tcp
