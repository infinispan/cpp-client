#include "hotrod/impl/operations/ClearOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

ClearOperation::ClearOperation(
    const Codec&      codec_,
    std::shared_ptr<TransportFactory> transportFactory_,
    const std::vector<char>&    cacheName_,
    Topology&  topologyId_,
    uint32_t    flags_)
    : RetryOnFailureOperation<std::vector<char>>(
        codec_, transportFactory_, cacheName_, topologyId_, flags_)
{}

std::vector<char> ClearOperation::executeOperation(infinispan::hotrod::transport::Transport& transport)
{
    TRACE("Executing Clear(flags=%u)", flags);
    std::unique_ptr<HeaderParams> params(RetryOnFailureOperation<std::vector<char>>::writeHeader(transport, CLEAR_REQUEST));
    transport.flush();
    RetryOnFailureOperation<std::vector<char>>::readHeaderAndValidate(transport, *params);
    TRACE("Finished Clear");
    return std::vector<char>();
}

}}} /// namespace infinispan::hotrod::operations
