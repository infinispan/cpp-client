#include <hotrod/impl/transport/tcp/InetSocketAddress.h>
#include "hotrod/impl/protocol/Codec22.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <hotrod/impl/TimeUnitParam.h>

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
    unsigned char timeCode = TimeUnitParam::encodeTimeUnits(lifespan,SECONDS,maxIdle,SECONDS);
    t.writeByte(timeCode);
    unsigned char lsTimeCode= timeCode >> 4;
    if (lsTimeCode!=DEFAULT && lsTimeCode!=INFINITE) {
        t.writeVLong(lifespan);
    }
    unsigned char miTimeCode= timeCode & 0x0F;
    if (miTimeCode!=DEFAULT && miTimeCode!=INFINITE) {
        t.writeVLong(maxIdle);
    }
}

}}} // namespace infinispan::hotrod::protocol
