#include "hotrod/impl/operations/ContainsKeyOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

ContainsKeyOperation::ContainsKeyOperation(
    const Codec&      codec_,
    std::shared_ptr<transport::TransportFactory> transportFactory_,
    const std::vector<char>&    key_,
    const std::vector<char>&    cacheName_,
    Topology&  topologyId_,
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
    if (HotRodConstants::isSuccess(status)) {
        containsKey = true;
        TRACE("Result is true");
    } else {
        TRACE("Error status %u", status);
    }
    return containsKey;
}

}}} /// namespace infinispan::hotrod::operations
