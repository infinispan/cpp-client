/*
 * RemoveClientListenerOperation.cpp
 *
 *  Created on: Sep 21, 2016
 *      Author: rigazilla
 */

#include <hotrod/impl/operations/RemoveClientListenerOperation.h>
#include <hotrod/impl/transport/TransportFactory.h>

namespace infinispan {
namespace hotrod {
namespace operations {


void RemoveClientListenerOperation::releaseTransport(transport::Transport* transport)
{
    std::dynamic_pointer_cast<TransportFactory>(transportFactory)->releaseTransport(*transport);
}

transport::Transport& RemoveClientListenerOperation::getTransport(int /*retryCount*/, const std::set<transport::InetSocketAddress>& /*failedServers*/)
{
    const TcpTransport &listenerTransport = (const TcpTransport&)listenerNotifier.findClientListenerTransport(clientListener.getListenerId());
    return std::dynamic_pointer_cast<TransportFactory>(transportFactory)->borrowTransportFromPool(listenerTransport.getServerAddress());
}

char RemoveClientListenerOperation::executeOperation(transport::Transport& transport)
{
    std::unique_ptr<HeaderParams> params(this->writeHeader(transport, REMOVE_CLIENT_LISTENER_REQUEST));
    transport.writeArray(clientListener.getListenerId());
    transport.flush();
    uint8_t status = readHeaderAndValidate(transport, *params);
    if (HotRodConstants::isSuccess(status))
    {
      listenerNotifier.releaseTransport(clientListener.getListenerId());
      listenerNotifier.removeClientListener(clientListener.getListenerId());
    }
    return status;
}

} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */
