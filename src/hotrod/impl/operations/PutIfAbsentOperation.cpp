

#include "hotrod/impl/operations/PutIfAbsentOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

PutIfAbsentOperation::PutIfAbsentOperation(
    const Codec& _codec, std::shared_ptr<transport::TransportFactory> _transportFactory, const std::vector<char>& _key,
    const std::vector<char>& _cacheName, Topology& _topologyId, uint32_t _flags,
    const std::vector<char>& _value, uint32_t _lifespan, uint32_t _maxIdle,
	EntryMediaTypes* df) :
        AbstractKeyValueOperation<std::vector<char>>(
            _codec, _transportFactory, _key, _cacheName, _topologyId,
            _flags, _value, _lifespan, _maxIdle, df)
{}

std::vector<char> PutIfAbsentOperation::executeOperation(Transport& transport)
{
    TRACE("Executing PutIfAbsent(flags=%u, lifespan=%u, maxIdle=%u)", flags, lifespan, maxIdle);
    TRACEBYTES("key = ", key);
    TRACEBYTES("value = ", value);
    uint8_t status = sendPutOperation(
        transport, PUT_IF_ABSENT_REQUEST, PUT_IF_ABSENT_RESPONSE);
    std::vector<char> previousValue;
    if (HotRodConstants::isNotExecuted(status)) {
        previousValue =
            AbstractKeyValueOperation<std::vector<char>>::returnPossiblePrevValue(transport, status);
    } else {
    	TRACE("Error status %u", status);
    }
    return previousValue;
}


}}} // namespace infinispan::hotrod::operations
