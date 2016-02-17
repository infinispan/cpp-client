

#include "hotrod/impl/operations/GetOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

GetOperation::GetOperation(
    const Codec& _codec, std::shared_ptr<transport::TransportFactory> _transportFactory, const std::vector<char>& _key,
    const std::vector<char>& _cacheName, IntWrapper& _topologyId, uint32_t _flags)
    : AbstractKeyOperation<std::vector<char>>(
        _codec, _transportFactory, _key, _cacheName, _topologyId, _flags)
{}

std::vector<char> GetOperation::executeOperation(Transport& transport) {
    std::vector<char> result;
    TRACE("Executing Get(flags=%u)", flags);
    TRACEBYTES("key = ", key);
    uint8_t status = sendKeyOperation(key, transport, GET_REQUEST, GET_RESPONSE);
    if (status == NO_ERROR_STATUS) {
        result = transport.readArray();
        TRACEBYTES("return value = ", result);
    } else {
        TRACE("Error status %u", status);
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations
