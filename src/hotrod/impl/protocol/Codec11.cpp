#include <infinispan/hotrod/InetSocketAddress.h>
#include "hotrod/impl/protocol/Codec11.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"
#include "hotrod/impl/consistenthash/ConsistentHash.h"
#include "hotrod/impl/consistenthash/ConsistentHashFactory.h"
#include "hotrod/sys/Log.h"

#include <set>
#include <map>
#include <vector>

namespace infinispan {
namespace hotrod {
using transport::InetSocketAddress;
using transport::Transport;

using consistenthash::ConsistentHash;
using consistenthash::ConsistentHashFactory;

namespace protocol {

std::map<InetSocketAddress, std::set<int32_t> > Codec11::computeNewHashes(
        infinispan::hotrod::transport::Transport& transport, uint32_t /*newTopologyId*/, int16_t /*numKeyOwners*/,
        uint8_t hashFunctionVersion, uint32_t /*hashSpace*/, uint32_t clusterSize) const {

    uint32_t numVirtualNodes = transport.readVInt();
    std::map<InetSocketAddress, std::set<int32_t> > servers2Hash;

    std::shared_ptr<ConsistentHash> ch;
    if (hashFunctionVersion != 0) {
        ch = transport.getTransportFactory().getConsistentHashFactory().newConsistentHash(hashFunctionVersion);
    } else {
        WARN("Could not obtain a consistent hash for version %d", hashFunctionVersion);
    }

    for (uint32_t i = 0; i < clusterSize; i++) {
        std::string host = transport.readString();
        int16_t port = transport.readUnsignedShort();
        // TODO: Performance improvement, since hash positions are fixed, we could
        //maybe only calculate for those nodes that the client is not aware of?
        int32_t baseHashCode = transport.read4ByteInt();
        int32_t normalizedHashCode = getNormalizedHash(baseHashCode, ch);

        cacheHashCode(servers2Hash, host, port, normalizedHashCode);
        if (numVirtualNodes > 1) {
            calcVirtualHashCodes(baseHashCode, numVirtualNodes, servers2Hash,
                    host, port, ch);
        }
    }

    return servers2Hash;
}

int32_t Codec11::getNormalizedHash(int32_t baseHashCode, std::shared_ptr<ConsistentHash> ch) const {
    if (ch != NULL) {
        return ch->getNormalizedHash(baseHashCode);
    } else {
        return baseHashCode;
    }
    return 1;
}

void Codec11::calcVirtualHashCodes(int32_t addrHashCode,
        uint32_t numVirtualNodes,
        std::map<InetSocketAddress, std::set<int32_t> >& servers2Hash,
        std::string host, int16_t port,
        std::shared_ptr<ConsistentHash> ch) const {

    for (uint32_t j = 1; j < numVirtualNodes; j++) {
        int32_t hashCode = calcVNodeHashCode(addrHashCode, j);
        cacheHashCode(servers2Hash, host, port, getNormalizedHash(hashCode, ch));
    }
}

void Codec11::cacheHashCode(
        std::map<InetSocketAddress, std::set<int32_t> > & servers2Hash,
        std::string host, int16_t port, int32_t hashCode) const {

    InetSocketAddress address(host, port);
    std::map<InetSocketAddress, std::set<int32_t> >::iterator it =
            servers2Hash.find(address);
    if (it == servers2Hash.end()) {
        std::set<int32_t> hashes;
        hashes.insert(hashCode);
        servers2Hash.insert(
                std::pair<InetSocketAddress, std::set<int32_t> >(address,
                        hashes));
    } else {
        it->second.insert(hashCode);
    }
}

int32_t Codec11::calcVNodeHashCode(int32_t addrHashCode, int32_t id) const {
    int32_t result = id;
    result = 31 * result + addrHashCode;
    return result;
}
}}} // namespace infinispan::hotrod::protocol
