#ifndef ISPN_HOTROD_PROTOCOL_CODEC12_H
#define ISPN_HOTROD_PROTOCOL_CODEC12_H



#include "hotrod/impl/protocol/Codec.h"

#include <stdint.h>

namespace infinispan {
namespace hotrod {

namespace transport {
class Transport;
}

namespace protocol {

class HeaderParams;

class Codec12 : public Codec
{
  public:
    HeaderParams& writeHeader(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params) const;

    uint8_t readHeader(
        infinispan::hotrod::transport::Transport& transport,
        const HeaderParams& params) const;

  protected:
    HeaderParams& writeHeader(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params, uint8_t version) const;

    void readNewTopologyIfPresent(
        infinispan::hotrod::transport::Transport& transport,
        const HeaderParams& params) const;

    void readNewTopologyAndHash(
        infinispan::hotrod::transport::Transport& transport,
        uint32_t topologyId) const;

    void checkForErrorsInResponseStatus(
        infinispan::hotrod::transport::Transport& transport,
        const HeaderParams& params, uint8_t status) const;

    // TODO : multithread management
    static long msgId;

  private:
    Codec12() {}

  friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC12_H
