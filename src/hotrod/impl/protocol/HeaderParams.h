#ifndef ISPN_HOTROD_PROTOCOL_HEADERPARAMS_H
#define ISPN_HOTROD_PROTOCOL_HEADERPARAMS_H

#include "infinispan/hotrod/Flag.h"
#include "hotrod/types.h"
#include "hotrod/impl/IntWrapper.h"

#include <set>

namespace infinispan {
namespace hotrod {
namespace protocol {

class HeaderParams
{
  public:
	HeaderParams(IntWrapper& topId);
    HeaderParams& setOpCode(uint8_t opCode);
    HeaderParams& setCacheName(const hrbytes& cacheName);
    HeaderParams& setFlags(uint32_t flags);
    HeaderParams& setClientIntel(uint8_t clientIntel);
    HeaderParams& setTxMarker(uint8_t txMarker);
    HeaderParams& setMessageId(uint64_t messageId);

  private:
    uint8_t toOpRespCode(uint8_t opCode);

    uint8_t opCode;
    uint8_t opRespCode;
    hrbytes cacheName;
    uint32_t flags;
    uint8_t clientIntel;
    uint8_t txMarker;
    IntWrapper& topologyId;
    uint64_t messageId;

    friend class Codec10;
    friend class Codec11;
    friend class Codec12;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_HEADERPARAMS_H
