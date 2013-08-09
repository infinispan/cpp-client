#ifndef ISPN_HOTROD_PROTOCOL_CODEC_H
#define ISPN_HOTROD_PROTOCOL_CODEC_H



#include <stdint.h>

namespace infinispan {
namespace hotrod {

namespace transport {
class Transport;
}

namespace protocol {

class HeaderParams;

class Codec
{
  public:
    virtual HeaderParams& writeHeader(
      transport::Transport& transport, HeaderParams& params) const = 0;

    virtual uint8_t readHeader(
      transport::Transport& transport, const HeaderParams& params) const = 0;

    virtual ~Codec() {}
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC_H
