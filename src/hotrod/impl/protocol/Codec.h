#ifndef ISPN_HOTROD_PROTOCOL_CODEC_H
#define ISPN_HOTROD_PROTOCOL_CODEC_H

#include "infinispan/hotrod/defs.h"
#include <set>
#include <map>

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

    virtual ~Codec() {}
  protected:

    virtual std::map<infinispan::hotrod::transport::InetSocketAddress, std::set<int32_t> > computeNewHashes(
               infinispan::hotrod::transport::Transport& transport, uint32_t newTopologyId, int16_t numKeyOwners,
               uint8_t hashFunctionVersion, uint32_t hashSpace, uint32_t clusterSize) const = 0;

};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC_H
