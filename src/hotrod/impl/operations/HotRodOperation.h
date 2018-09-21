#ifndef ISPN_HOTROD_OPERATIONS_HOTRODOPERATION_H
#define ISPN_HOTROD_OPERATIONS_HOTRODOPERATION_H

#include "hotrod/sys/Log.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/protocol/Codec20.h"
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
        uint32_t _flags, const std::vector<char>& _cacheName, Topology& _topologyId, EntryMediaTypes* df) :
            codec(_codec), flags(_flags),
            cacheName(_cacheName), topologyId(_topologyId), dataFormat(df) {
    }

    protocol::HeaderParams* writeHeader(
        transport::Transport& transport, uint8_t opCode)
    {
        protocol::HeaderParams* params =
            new protocol::HeaderParams(topologyId);
        (*params).setOpCode(opCode).setCacheName(cacheName)
            .setFlags(flags).setClientIntel(CLIENT_INTELLIGENCE_HASH_DISTRIBUTION_AWARE)
            .setTxMarker(NO_TX).setTopologyAge(0).setDataFormat(dataFormat);
        codec.writeHeader(transport, *params);
        return params;
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
    std::vector<char> cacheName;
    // TODO: atomic
    Topology& topologyId;
    void setDataFormat(EntryMediaTypes *df) { this->dataFormat = df; }
    EntryMediaTypes* dataFormat;
  private:
    static const uint8_t NO_TX = 0x0;

};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_HOTRODOPERATION_H
