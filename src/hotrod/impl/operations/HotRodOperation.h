#ifndef ISPN_HOTROD_OPERATIONS_HOTRODOPERATION_H
#define ISPN_HOTROD_OPERATIONS_HOTRODOPERATION_H



#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/protocol/Codec.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "infinispan/hotrod/Flag.h"

namespace infinispan {
namespace hotrod {
namespace operations {


template<class T> class HotRodOperation : public protocol::HotRodConstants
{
  public:
    virtual T execute() = 0;

  protected:
    HotRodOperation(
        const protocol::Codec& _codec,
        uint32_t _flags, const hrbytes& _cacheName, uint32_t _topologyId) :
            codec(_codec), flags(_flags),
            cacheName(_cacheName), topologyId(_topologyId) {
    }

    protocol::HeaderParams& writeHeader(
        transport::Transport& transport, uint8_t opCode)
    {
       // TODO: CLIENT_INTELLIGENCE_HASH_DISTRIBUTION_AWARE
        protocol::HeaderParams* params =
            new protocol::HeaderParams();
        (*params).setOpCode(opCode).setCacheName(cacheName)
            .setFlags(flags).setClientIntel(CLIENT_INTELLIGENCE_BASIC)
            .setTopologyId(topologyId).setTxMarker(NO_TX);

        return codec.writeHeader(transport, *params);
    }

    uint8_t readHeaderAndValidate(
        transport::Transport& transport,
        protocol::HeaderParams& params)
    {
        return codec.readHeader(transport, params);
    }

    virtual ~HotRodOperation() {}

    const protocol::Codec& codec;
    uint32_t flags;
    hrbytes cacheName;
    // TODO: atomic
    uint32_t topologyId;

  private:
    static const uint8_t NO_TX = 0x0;

};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_HOTRODOPERATION_H
