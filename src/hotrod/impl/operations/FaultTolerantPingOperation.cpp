#include "hotrod/impl/operations/FaultTolerantPingOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

FaultTolerantPingOperation::FaultTolerantPingOperation(
    const Codec&      codec_,
    std::shared_ptr<TransportFactory> transportFactory_,
    const hrbytes&    cacheName_,
    IntWrapper&  topologyId_,
    uint32_t    flags_)
    : RetryOnFailureOperation<PingResult>(
        codec_, transportFactory_, cacheName_, topologyId_, flags_)
{}

Transport& FaultTolerantPingOperation::getTransport(int /*retryCount*/)
{
        return RetryOnFailureOperation<PingResult>::transportFactory->getTransport(cacheName);
}

PingResult FaultTolerantPingOperation::executeOperation(transport::Transport& transport)
{
    PingOperation pingOp(codec, topologyId, transport, cacheName);
    return pingOp.execute();
}

}}} /// namespace infinispan::hotrod::operations
