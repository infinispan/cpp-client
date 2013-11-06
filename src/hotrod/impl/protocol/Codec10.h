#ifndef ISPN_HOTROD_PROTOCOL_CODEC10_H
#define ISPN_HOTROD_PROTOCOL_CODEC10_H



#include "hotrod/impl/protocol/Codec.h"

namespace infinispan {
namespace hotrod {

namespace transport {
class Transport;
}

namespace protocol {

class HeaderParams;

class Codec10 : public Codec
{
  public:
    HeaderParams& writeHeader(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params) const;

    uint8_t readHeader(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params) const;

  protected:
    HeaderParams& writeHeader(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params, uint8_t version) const;

    std::map<infinispan::hotrod::transport::InetSocketAddress, std::set<int32_t> > computeNewHashes(
        infinispan::hotrod::transport::Transport& transport, uint32_t newTopologyId, int16_t numKeyOwners,
        uint8_t hashFunctionVersion, uint32_t hashSpace, uint32_t clusterSize) const;

    // TODO : multithread management
    static long msgId;

    Codec10() {}

  friend class CodecFactory;

  private:

    void readNewTopologyIfPresent(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params) const;

    void readNewTopologyAndHash(
        infinispan::hotrod::transport::Transport& transport,
        uint32_t& topologyId) const;

    void checkForErrorsInResponseStatus(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params, uint8_t status) const;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC12_H
