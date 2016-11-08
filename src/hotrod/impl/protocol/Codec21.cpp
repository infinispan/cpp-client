#include <infinispan/hotrod/InetSocketAddress.h>
#include "hotrod/impl/protocol/Codec21.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/TransportFactory.h"
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


void Codec21::writeClientListenerParams(transport::Transport& t, const ClientListener& clientListener,
		const std::vector<std::vector<char> > &filterFactoryParams, const std::vector<std::vector<char> > &converterFactoryParams) const
{
	Codec20::writeClientListenerParams(t,clientListener, filterFactoryParams, converterFactoryParams);
	t.writeByte((short)(clientListener.useRawData ? 1 : 0));
}

ClientCacheEntryExpiredEvent<std::vector<char>> Codec21::readExpiredEvent(transport::Transport &transport) const
{
    std::vector<char> key = transport.readArray();
    return ClientCacheEntryExpiredEvent<std::vector<char>>(key);
}


}}} // namespace infinispan::hotrod::protocol
