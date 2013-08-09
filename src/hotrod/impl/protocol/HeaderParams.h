#ifndef ISPN_HOTROD_PROTOCOL_HEADERPARAMS_H
#define ISPN_HOTROD_PROTOCOL_HEADERPARAMS_H



#include "hotrod/sys/types.h"
#include "infinispan/hotrod/Flag.h"

#include <set>

namespace infinispan {
namespace hotrod {
namespace protocol {


class HeaderParams
{
  public:
    HeaderParams& setOpCode(uint8_t opCode);
    HeaderParams& setCacheName(const hrbytes& cacheName);
    HeaderParams& setFlags(uint32_t flags);
    HeaderParams& setClientIntel(uint8_t clientIntel);
    HeaderParams& setTxMarker(uint8_t txMarker);
    HeaderParams& setTopologyId(uint32_t topologyId);
    HeaderParams& setMessageId(uint64_t messageId);

  private:
    uint8_t toOpRespCode(uint8_t opCode);

    uint8_t opCode;
    uint8_t opRespCode;
    hrbytes cacheName;
    uint32_t flags;
    uint8_t clientIntel;
    uint8_t txMarker;
    uint32_t topologyId;
    uint64_t messageId;

    friend class Codec12;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_HEADERPARAMS_H
