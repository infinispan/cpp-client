#ifndef ISPN_HOTROD_PROTOCOL_CODEC_H
#define ISPN_HOTROD_PROTOCOL_CODEC_H

#include "infinispan/hotrod/defs.h"
#include <set>
#include <map>
#include <vector>
namespace infinispan {
namespace hotrod {

namespace transport {
class Transport;
class InetSocketAddress;
}

namespace protocol {

class HeaderParams;

class Codec
{
  public:
    virtual HeaderParams& writeHeader(
      transport::Transport& transport, HeaderParams& params) const = 0;

    virtual uint8_t readHeader(
      transport::Transport& transport, HeaderParams& params) const = 0;

    virtual std::vector<char> returnPossiblePrevValue(transport::Transport& t, uint8_t status, uint32_t flags) const = 0;
    virtual void writeExpirationParams(transport::Transport& t,uint64_t lifespan, uint64_t maxIdle) const = 0;
    virtual ~Codec() {}
  protected:

};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC_H
