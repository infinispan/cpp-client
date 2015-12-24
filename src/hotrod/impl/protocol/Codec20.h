#ifndef ISPN_HOTROD_PROTOCOL_CODEC20_H
#define ISPN_HOTROD_PROTOCOL_CODEC20_H

#include "hotrod/types.h"
#include "hotrod/impl/protocol/Codec.h"

namespace infinispan {
namespace hotrod {
class IntWrapper;

namespace transport {
class Transport;
}

namespace protocol {

class HeaderParams;

class Codec20 : public Codec
{
  public:
    HeaderParams& writeHeader(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params) const;

    uint8_t readHeader(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params) const;

    long getMessageId();

  protected:
    HeaderParams& writeHeader(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params, uint8_t version) const;

    std::map<infinispan::hotrod::transport::InetSocketAddress, std::set<int32_t> > computeNewHashes(
        infinispan::hotrod::transport::Transport& transport, uint32_t newTopologyId, int16_t numKeyOwners,
        uint8_t hashFunctionVersion, uint32_t hashSpace, uint32_t clusterSize) const;

    Codec20() {}

  friend class CodecFactory;

  private:

    void readNewTopologyIfPresent(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params) const;

    void readNewTopologyAndHash(
        infinispan::hotrod::transport::Transport& transport,
        IntWrapper& topologyId,
        const hrbytes& cacheName) const;

    void checkForErrorsInResponseStatus(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params, uint8_t status) const;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC20_H
