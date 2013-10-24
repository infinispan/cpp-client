#include "hotrod/sys/types.h"
#include "hotrod/impl/operations/GetWithVersionOperation.h"
#include "hotrod/impl/VersionedValueImpl.h"
#include <cstring>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

GetWithVersionOperation::GetWithVersionOperation(
    const Codec&      codec_,
    HR_SHARED_PTR<TransportFactory> transportFactory_,
    const hrbytes&    key_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    uint32_t    flags_)
    : AbstractKeyOperation<VersionedValueImpl<hrbytes> >(
        codec_, transportFactory_, key_, cacheName_, topologyId_, flags_)
{}

VersionedValueImpl<hrbytes> GetWithVersionOperation::executeOperation(Transport& transport)
{
    VersionedValueImpl<hrbytes> result;
    uint8_t status = sendKeyOperation(
        key, transport, GET_WITH_VERSION_REQUEST, GET_WITH_VERSION_RESPONSE);
    if (status == NO_ERROR_STATUS) {
        result.setVersion(transport.readLong());
        result.setValue(transport.readArray());
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations
