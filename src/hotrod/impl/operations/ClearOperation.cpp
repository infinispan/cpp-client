

#include "infinispan/hotrod/types.h"
#include "hotrod/impl/operations/ClearOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

ClearOperation::ClearOperation(
    const Codec&      codec_,
    HR_SHARED_PTR<TransportFactory> transportFactory_,
    const hrbytes&    cacheName_,
    IntWrapper&  topologyId_,
    uint32_t    flags_)
    : RetryOnFailureOperation<hrbytes>(
        codec_, transportFactory_, cacheName_, topologyId_, flags_)
{}

Transport& ClearOperation::getTransport(int /*retryCount*/)
{
        return RetryOnFailureOperation<hrbytes>::transportFactory->getTransport();
}

hrbytes ClearOperation::executeOperation(infinispan::hotrod::transport::Transport& transport)
{
    TRACE("Executing Clear(flags=%u)", flags);
    hr_scoped_ptr<HeaderParams> params(&(RetryOnFailureOperation<hrbytes>::writeHeader(transport, CLEAR_REQUEST)));
    transport.flush();
    RetryOnFailureOperation<hrbytes>::readHeaderAndValidate(transport, *params);
    TRACE("Finished Clear");
    return hrbytes();
}

}}} /// namespace infinispan::hotrod::operations
