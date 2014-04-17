

#include "infinispan/hotrod/types.h"
#include "hotrod/impl/operations/ContainsKeyOperation.h"
#include <cstring>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

ContainsKeyOperation::ContainsKeyOperation(
    const Codec&      codec_,
    HR_SHARED_PTR<transport::TransportFactory> transportFactory_,
    const hrbytes&    key_,
    const hrbytes&    cacheName_,
    IntWrapper&  topologyId_,
    uint32_t    flags_)
    : AbstractKeyOperation<bool>(
        codec_, transportFactory_, key_, cacheName_, topologyId_, flags_)
{}

bool ContainsKeyOperation::executeOperation(Transport& transport)
{
    TRACE("Execute ContainsKey(flags=%u)", flags);
    TRACEBYTES("key = ", key);
    bool containsKey = false;
    uint8_t status = sendKeyOperation(key,
        transport, CONTAINS_KEY_REQUEST, CONTAINS_KEY_RESPONSE);
    if (status == NO_ERROR_STATUS) {
        containsKey = true;
        TRACE("Result is true");
    } else {
        TRACE("Error status %u", status);
    }
    return containsKey;
}

}}} /// namespace infinispan::hotrod::operations
