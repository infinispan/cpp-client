#include "hotrod/impl/protocol/Codec22.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"

#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <vector>

namespace infinispan {
namespace hotrod {

using transport::Transport;
using transport::InetSocketAddress;
using transport::TransportFactory;

namespace protocol {

HeaderParams& Codec22::writeHeader(Transport& transport, HeaderParams& params) const {
    return Codec20::writeHeader(transport, params, HotRodConstants::VERSION_22);
}
void Codec22::writeExpirationParams(transport::Transport& t,uint64_t lifespan, uint64_t maxIdle) const {
	t.writeVLong(lifespan);
    t.writeVLong(maxIdle);
}

}}} // namespace infinispan::hotrod::protocol
