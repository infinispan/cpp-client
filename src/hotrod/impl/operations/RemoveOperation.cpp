#include "hotrod/impl/operations/RemoveOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

RemoveOperation::RemoveOperation(
    const Codec&      codec_,
    std::shared_ptr<transport::TransportFactory> transportFactory_,
    const std::vector<char>&    key_,
    const std::vector<char>&    cacheName_,
    Topology&  topologyId_,
    uint32_t    flags_,
    EntryMediaTypes* df)
    : AbstractKeyOperation<std::vector<char>>(
        codec_, transportFactory_, key_, cacheName_, topologyId_, flags_, df)
{}

std::vector<char> RemoveOperation::executeOperation(Transport& transport)
{
    TRACE("Execute Remove(flags=%u)", flags);
    TRACEBYTES("key = ", key);
    std::vector<char> result;
    uint8_t status=sendKeyOperation(key,
        transport, REMOVE_REQUEST, REMOVE_RESPONSE);
    return AbstractKeyOperation<std::vector<char>>::returnPossiblePrevValue(transport, status);
}

}}} /// namespace infinispan::hotrod::operations
