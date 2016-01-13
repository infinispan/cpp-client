#include "hotrod/impl/operations/RemoveOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

RemoveOperation::RemoveOperation(
    const Codec&      codec_,
    std::shared_ptr<transport::TransportFactory> transportFactory_,
    const hrbytes&    key_,
    const hrbytes&    cacheName_,
    IntWrapper&  topologyId_,
    uint32_t    flags_)
    : AbstractKeyOperation<hrbytes>(
        codec_, transportFactory_, key_, cacheName_, topologyId_, flags_)
{}

hrbytes RemoveOperation::executeOperation(Transport& transport)
{
    TRACE("Execute Remove(flags=%u)", flags);
    TRACEBYTES("key = ", key);
    hrbytes result;
    uint8_t status=sendKeyOperation(key,
        transport, REMOVE_REQUEST, REMOVE_RESPONSE);
    return AbstractKeyOperation<hrbytes>::returnPossiblePrevValue(transport,status);
}

}}} /// namespace infinispan::hotrod::operations
