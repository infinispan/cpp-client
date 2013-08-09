

#include "hotrod/sys/types.h"
#include "hotrod/impl/operations/ContainsKeyOperation.h"
#include <cstring>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

ContainsKeyOperation::ContainsKeyOperation(
    const Codec&      codec_,
    infinispan::hotrod::transport::TransportFactory* transportFactory_,
    const hrbytes&    key_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    uint32_t    flags_)
    : AbstractKeyOperation<bool>(
        codec_, transportFactory_, key_, cacheName_, topologyId_, flags_)
{}

bool ContainsKeyOperation::executeOperation(Transport& transport)
{
    bool containsKey = false;
    uint8_t status = sendKeyOperation(key,
        transport, CONTAINS_KEY_REQUEST, CONTAINS_KEY_RESPONSE);
    if (status == NO_ERROR_STATUS) {
        containsKey = true;
    }
    return containsKey;
}

}}} /// namespace infinispan::hotrod::operations
