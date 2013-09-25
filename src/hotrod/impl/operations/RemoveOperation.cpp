

#include "hotrod/sys/types.h"
#include "hotrod/impl/operations/RemoveOperation.h"
#include <cstring>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

RemoveOperation::RemoveOperation(
    const Codec&      codec_,
    HR_SHARED_PTR<transport::TransportFactory> transportFactory_,
    const hrbytes&    key_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    uint32_t    flags_)
    : AbstractKeyOperation<hrbytes>(
        codec_, transportFactory_, key_, cacheName_, topologyId_, flags_)
{}

hrbytes RemoveOperation::executeOperation(Transport& transport)
{
    hrbytes result;
    uint8_t status = sendKeyOperation(key,
        transport, REMOVE_REQUEST, REMOVE_RESPONSE);
    if (status != KEY_DOES_NOT_EXIST_STATUS) {
        result = AbstractKeyOperation<hrbytes>::returnPossiblePrevValue(transport);
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations
