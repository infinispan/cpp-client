#include "hotrod/impl/operations/SizeOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

SizeOperation::SizeOperation(
    const Codec&      codec_,
    std::shared_ptr<TransportFactory> transportFactory_,
    const std::vector<char>&    cacheName_,
    Topology&  topologyId_,
    uint32_t    flags_)
    : RetryOnFailureOperation<uint64_t>(
        codec_, transportFactory_, cacheName_, topologyId_, flags_)
{}

Transport& SizeOperation::getTransport(int /*retryCount*/)
{
        return RetryOnFailureOperation<uint64_t>::transportFactory->getTransport(cacheName);
}

uint64_t SizeOperation::executeOperation(infinispan::hotrod::transport::Transport& transport)
{
    TRACE("Executing size(flags=%u)", flags);
    std::unique_ptr<HeaderParams> params(&(RetryOnFailureOperation<uint64_t>::writeHeader(transport, SIZE_REQUEST)));
    transport.flush();
    RetryOnFailureOperation<uint64_t>::readHeaderAndValidate(transport, *params);
    TRACE("Finished Size");
    return transport.readVInt();
}

}}} /// namespace infinispan::hotrod::operations
