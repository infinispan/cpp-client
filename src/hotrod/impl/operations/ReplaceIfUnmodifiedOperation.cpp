#include "hotrod/impl/operations/ReplaceIfUnmodifiedOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

ReplaceIfUnmodifiedOperation::ReplaceIfUnmodifiedOperation(
    const Codec& _codec, std::shared_ptr<TransportFactory> _transportFactory, const std::vector<char>& _key,
    const std::vector<char>& _cacheName, IntWrapper& _topologyId, uint32_t _flags,
    const std::vector<char>& _value, uint32_t _lifespan, uint32_t _maxIdle, int64_t _version) :
        AbstractKeyValueOperation<VersionedOperationResponse>(
            _codec, _transportFactory, _key, _cacheName, _topologyId,
            _flags, _value, _lifespan, _maxIdle), version(_version)
{}

VersionedOperationResponse ReplaceIfUnmodifiedOperation::executeOperation(
    Transport& transport)
{
    TRACE("Execute ReplaceIfUnmodified(flags=%d, lifespan=%d, maxIdle=%d, version=%lld)", flags, lifespan, maxIdle, version);
    TRACEBYTES("key = ", key);
    TRACEBYTES("value = ", value);
    // 1) write header
    std::unique_ptr<infinispan::hotrod::protocol::HeaderParams> params(
        &(AbstractKeyOperation<VersionedOperationResponse>::writeHeader(
            transport, REPLACE_IF_UNMODIFIED_REQUEST)));

    //2) write message body
    transport.writeArray(key);
    codec.writeExpirationParams(transport, lifespan, maxIdle);
    transport.writeLong(version);
    transport.writeArray(value);
    transport.flush();

    return AbstractKeyValueOperation<VersionedOperationResponse>::returnVersionedOperationResponse(transport, *params);
}


}}} // namespace infinispan::hotrod::operations
