#include "hotrod/impl/operations/ReplaceOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

ReplaceOperation::ReplaceOperation(
    const Codec& _codec, std::shared_ptr<transport::TransportFactory> _transportFactory, const std::vector<char>& _key,
    const std::vector<char>& _cacheName, Topology& _topologyId, uint32_t _flags,
    const std::vector<char>& _value, uint32_t _lifespan, uint32_t _maxIdle) :
        AbstractKeyValueOperation<std::vector<char>>(
            _codec, _transportFactory, _key, _cacheName, _topologyId,
            _flags, _value, _lifespan, _maxIdle)
{}

std::vector<char> ReplaceOperation::executeOperation(Transport& transport)
{
    TRACE("Execute Replace(flags=%u, lifespan=%u, maxIdle=%u)", flags, lifespan, maxIdle);
    TRACEBYTES("key = ", key);
    TRACEBYTES("value = ", value);
    std::vector<char> previousValue;
    uint8_t status = sendPutOperation(
        transport, REPLACE_REQUEST, REPLACE_RESPONSE);
    if (status == NO_ERROR_STATUS || status == NOT_PUT_REMOVED_REPLACED_STATUS) {
        previousValue =
            AbstractKeyValueOperation<std::vector<char>>::returnPossiblePrevValue(transport,status);
    } else {
        TRACE("Error status %u", status);
    }
    return previousValue;
}


}}} // namespace infinispan::hotrod::operations
