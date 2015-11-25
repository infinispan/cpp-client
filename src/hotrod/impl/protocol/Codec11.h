#ifndef ISPN_HOTROD_PROTOCOL_CODEC11_H
#define ISPN_HOTROD_PROTOCOL_CODEC11_H

#include "hotrod/impl/protocol/Codec10.h"
#include <string>

namespace infinispan {
namespace hotrod {
namespace consistenthash {
class ConsistentHash;
}
namespace protocol {

class Codec11: public Codec10 {

public:
    HeaderParams& writeHeader(
                infinispan::hotrod::transport::Transport& transport,
                HeaderParams& params) const;
protected:

    std::map<infinispan::hotrod::transport::InetSocketAddress, std::set<int32_t> > computeNewHashes(
            infinispan::hotrod::transport::Transport& transport, uint32_t newTopologyId, int16_t numKeyOwners,
            uint8_t hashFunctionVersion, uint32_t hashSpace, uint32_t clusterSize) const;

private:

    int32_t
    getNormalizedHash(int32_t baseHashCode,
            std::shared_ptr<infinispan::hotrod::consistenthash::ConsistentHash> ch) const;

    void calcVirtualHashCodes(
            int32_t addrHashCode,
            uint32_t numVirtualNodes,
            std::map<infinispan::hotrod::transport::InetSocketAddress,
                    std::set<int32_t> >& servers2Hash, std::string host,
            int16_t port,
            std::shared_ptr<infinispan::hotrod::consistenthash::ConsistentHash> ch) const;

    void cacheHashCode(
            std::map<infinispan::hotrod::transport::InetSocketAddress,
                    std::set<int32_t> > & servers2Hash, std::string host,
            int16_t port, int32_t hashCode) const;

    int32_t calcVNodeHashCode(int32_t addrHashCode, int32_t id) const;

protected:

    Codec11() {
    }

    friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol
#endif // ISPN_HOTROD_PROTOCOL_CODEC12_H
