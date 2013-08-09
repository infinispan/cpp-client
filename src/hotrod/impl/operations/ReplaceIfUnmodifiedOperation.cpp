

#include "hotrod/impl/operations/ReplaceIfUnmodifiedOperation.h"
#include "hotrod/sys/types.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

ReplaceIfUnmodifiedOperation::ReplaceIfUnmodifiedOperation(
    const Codec& _codec, TransportFactory* _transportFactory, const hrbytes& _key,
    const hrbytes& _cacheName, uint32_t _topologyId, uint32_t _flags,
    const hrbytes& _value, uint32_t _lifespan, uint32_t _maxIdle, int64_t _version) :
        AbstractKeyValueOperation<VersionedOperationResponse>(
            _codec, _transportFactory, _key, _cacheName, _topologyId,
            _flags, _value, _lifespan, _maxIdle), version(_version)
{}

VersionedOperationResponse ReplaceIfUnmodifiedOperation::executeOperation(
    Transport& transport)
{
    // 1) write header
    hr_scoped_ptr<infinispan::hotrod::protocol::HeaderParams> params(
        &(AbstractKeyOperation<VersionedOperationResponse>::writeHeader(
            transport, REPLACE_IF_UNMODIFIED_REQUEST)));

    //2) write message body
    transport.writeArray(key);
    transport.writeVInt(lifespan);
    transport.writeVInt(maxIdle);
    transport.writeLong(version);
    transport.writeArray(value);
    transport.flush();

    return AbstractKeyValueOperation<VersionedOperationResponse>::returnVersionedOperationResponse(transport, *params);
}


}}} // namespace infinispan::hotrod::operations
