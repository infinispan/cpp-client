#include "hotrod/impl/protocol/Codec26.h"
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
#include <infinispan/hotrod/InetSocketAddress.h>

namespace infinispan {
namespace hotrod {

using transport::Transport;
using transport::InetSocketAddress;
using transport::TransportFactory;

namespace protocol {
    void Codec26::writeClientListenerInterests(transport::Transport& t, unsigned char interestFlag) const
    {
        t.writeVInt(interestFlag);
    }

}}} // namespace infinispan::hotrod::protocol
