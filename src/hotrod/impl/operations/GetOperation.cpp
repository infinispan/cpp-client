

#include "infinispan/hotrod/types.h"
#include "hotrod/impl/operations/GetOperation.h"
#include <cstring>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

GetOperation::GetOperation(
    const Codec& _codec, HR_SHARED_PTR<transport::TransportFactory> _transportFactory, const hrbytes& _key,
    const hrbytes& _cacheName, IntWrapper& _topologyId, uint32_t _flags)
    : AbstractKeyOperation<hrbytes>(
        _codec, _transportFactory, _key, _cacheName, _topologyId, _flags)
{}

hrbytes GetOperation::executeOperation(Transport& transport) {
    hrbytes result;
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
