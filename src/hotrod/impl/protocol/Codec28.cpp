#include "hotrod/impl/protocol/Codec28.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/sys/Mutex.h"
#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <hotrod/impl/TimeUnitParam.h>
#include <infinispan/hotrod/InetSocketAddress.h>

namespace infinispan {
namespace hotrod {

using transport::Transport;

namespace protocol {

void writeMediaType(Transport& transport, const MediaType& mediaType) {
    if (!mediaType.type.empty()) {
        auto id = IdsMediaTypes::getByMediaType(mediaType.toString());
        if ( id != -1) {
            transport.writeByte(1);
            transport.writeVInt((unsigned int)id);
        } else {
            transport.writeByte(2);
            transport.writeString(mediaType.toString());
        }
        transport.writeVInt(mediaType.params.size());
        for (auto p : mediaType.params) {
            transport.writeString(p.first);
            transport.writeString(p.second);
        }
    } else {
        transport.writeByte(0);
    }
}
    HeaderParams& Codec28::writeHeader(Transport& transport, HeaderParams& params) const
    {
        Codec20::writeHeader(transport, params);
        // Write media type for key
        auto format = params.dataFormat;
        writeMediaType(transport, format.keyMediaType);
        writeMediaType(transport, format.valueMediaType);
        return params;
    }
}}} // namespace infinispan::hotrod::protocol
