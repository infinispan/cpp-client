#ifndef ISPN_HOTROD_SYS_SOCKET_H
#define ISPN_HOTROD_SYS_SOCKET_H



#include "infinispan/hotrod/types.h"

namespace infinispan {
namespace hotrod {
namespace sys {

class Socket
{
  public:
    static Socket* create();

    virtual void connect(const std::string& host, int port) = 0;
    virtual void close() = 0;
    virtual size_t read(char *p, size_t n) = 0;
    virtual void write(const char *p, size_t n) = 0;
};

}}} // namespace

#endif  /* ISPN_HOTROD_SYS_SOCKET_H */
