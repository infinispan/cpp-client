

#include "infinispan/hotrod/types.h"
#include "hotrod/impl/operations/FaultTolerantPingOperation.h"
#include "hotrod/impl/operations/PingOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

FaultTolerantPingOperation::FaultTolerantPingOperation(
    const Codec&      codec_,
    HR_SHARED_PTR<TransportFactory> transportFactory_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    uint32_t    flags_)
    : RetryOnFailureOperation<PingResult>(
        codec_, transportFactory_, cacheName_, topologyId_, flags_)
{}

Transport& FaultTolerantPingOperation::getTransport(int /*retryCount*/)
{
        return RetryOnFailureOperation<PingResult>::transportFactory->getTransport();
}

PingResult FaultTolerantPingOperation::executeOperation(transport::Transport& transport)
{
    PingOperation pingOp(codec, topologyId, transport, cacheName);
    return pingOp.execute();
}

}}} /// namespace infinispan::hotrod::operations
