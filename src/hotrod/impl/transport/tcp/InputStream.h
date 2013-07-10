#ifndef ISPN_HOTROD_TRANSPORT_TCP_INPUTSTREAM_H
#define ISPN_HOTROD_TRANSPORT_TCP_INPUTSTREAM_H

#include <sstream>

#include "hotrod/sys/types.h"
#include "hotrod/sys/Socket.h"

namespace infinispan {
namespace hotrod {
namespace transport {

class InputStream
{
  public:
    InputStream(sys::Socket& socket);

    static const size_t BufferSize = 1024;
    void read(char* buffer, size_t size);
    char read();

  private:
    sys::Socket& socket;
    char buffer[BufferSize];
    char* ptr;
    bool hasMore;
    size_t capacity;
};

InputStream& operator>>(InputStream& in, char& c);

}}} // namespace infinispan::hotrod::transport::tcp

#endif  // ISPN_HOTROD_TRANSPORT_TCP_INPUTSTREAM_H

