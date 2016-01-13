

#include "hotrod/impl/operations/PutIfAbsentOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

PutIfAbsentOperation::PutIfAbsentOperation(
    const Codec& _codec, std::shared_ptr<transport::TransportFactory> _transportFactory, const hrbytes& _key,
    const hrbytes& _cacheName, IntWrapper& _topologyId, uint32_t _flags,
    const hrbytes& _value, uint32_t _lifespan, uint32_t _maxIdle) :
        AbstractKeyValueOperation<hrbytes>(
            _codec, _transportFactory, _key, _cacheName, _topologyId,
            _flags, _value, _lifespan, _maxIdle)
{}

hrbytes PutIfAbsentOperation::executeOperation(Transport& transport)
{
    TRACE("Executing PutIfAbsent(flags=%u, lifespan=%u, maxIdle=%u)", flags, lifespan, maxIdle);
    TRACEBYTES("key = ", key);
    TRACEBYTES("value = ", value);
    uint8_t status = sendPutOperation(
        transport, PUT_IF_ABSENT_REQUEST, PUT_IF_ABSENT_RESPONSE);
    hrbytes previousValue;
    if (status == NO_ERROR_STATUS || status == NOT_PUT_REMOVED_REPLACED_STATUS) {
        previousValue =
            AbstractKeyValueOperation<hrbytes>::returnPossiblePrevValue(transport,status);
    } else {
    	TRACE("Error status %u", status);
    }
    return previousValue;
}


}}} // namespace infinispan::hotrod::operations
