

#include "hotrod/impl/operations/PutIfAbsentOperation.h"
#include "infinispan/hotrod/types.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

PutIfAbsentOperation::PutIfAbsentOperation(
    const Codec& _codec, HR_SHARED_PTR<transport::TransportFactory> _transportFactory, const hrbytes& _key,
    const hrbytes& _cacheName, uint32_t _topologyId, uint32_t _flags,
    const hrbytes& _value, uint32_t _lifespan, uint32_t _maxIdle) :
        AbstractKeyValueOperation<hrbytes>(
            _codec, _transportFactory, _key, _cacheName, _topologyId,
            _flags, _value, _lifespan, _maxIdle)
{}

hrbytes PutIfAbsentOperation::executeOperation(Transport& transport)
{
    uint8_t status = sendPutOperation(
        transport, PUT_IF_ABSENT_REQUEST, PUT_IF_ABSENT_RESPONSE);
    hrbytes previousValue;
    if (status == NO_ERROR_STATUS || status == NOT_PUT_REMOVED_REPLACED_STATUS) {
        previousValue =
            AbstractKeyValueOperation<hrbytes>::returnPossiblePrevValue(transport);
    }
    return previousValue;
}


}}} // namespace infinispan::hotrod::operations
