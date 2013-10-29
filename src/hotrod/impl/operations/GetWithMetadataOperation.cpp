

#include "infinispan/hotrod/types.h"
#include "hotrod/impl/operations/GetWithMetadataOperation.h"
#include "hotrod/impl/MetadataValueImpl.h"
#include <cstring>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

GetWithMetadataOperation::GetWithMetadataOperation(
    const Codec&      codec_,
    HR_SHARED_PTR<TransportFactory> transportFactory_,
    const hrbytes&    key_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    uint32_t    flags_)
    : AbstractKeyOperation<MetadataValueImpl<hrbytes> >(
        codec_, transportFactory_, key_, cacheName_, topologyId_, flags_)
{}

MetadataValueImpl<hrbytes> GetWithMetadataOperation::executeOperation(Transport& transport)
{
    MetadataValueImpl<hrbytes> result;
    uint8_t status = sendKeyOperation(
        key, transport, GET_WITH_METADATA_REQUEST, GET_WITH_METADATA_RESPONSE);
    if (status == NO_ERROR_STATUS) {
        uint8_t flag = transport.readByte();
        if ((flag & INFINITE_LIFESPAN) != INFINITE_LIFESPAN) {
        	result.setCreated(transport.readLong());
        	result.setLifespan(transport.readVInt());
        }
        if ((flag & INFINITE_MAXIDLE) != INFINITE_MAXIDLE) {
            result.setLastUsed(transport.readLong());
            result.setMaxIdle(transport.readVInt());
        }
        result.setVersion(transport.readLong());
        result.setValue(transport.readArray());
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations
