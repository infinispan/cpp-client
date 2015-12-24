#include "hotrod/impl/protocol/Codec23.h"
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

HeaderParams& Codec23::writeHeader(Transport& transport, HeaderParams& params) const {
    return Codec20::writeHeader(transport, params, HotRodConstants::VERSION_23);
}

}}} // namespace infinispan::hotrod::protocol
