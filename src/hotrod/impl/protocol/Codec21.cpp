#include "hotrod/impl/protocol/Codec21.h"
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

HeaderParams& Codec21::writeHeader(Transport& transport, HeaderParams& params) const {
    return Codec20::writeHeader(transport, params, HotRodConstants::VERSION_21);
}

}}} // namespace infinispan::hotrod::protocol
