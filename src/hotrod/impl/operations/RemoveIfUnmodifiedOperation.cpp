

#include "hotrod/sys/types.h"
#include "hotrod/impl/operations/RemoveIfUnmodifiedOperation.h"
#include <cstring>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

RemoveIfUnmodifiedOperation::RemoveIfUnmodifiedOperation(
    const Codec&      codec_,
    HR_SHARED_PTR<transport::TransportFactory> transportFactory_,
    const hrbytes&    key_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    uint32_t    flags_,
    int64_t version_)
    : AbstractKeyOperation<VersionedOperationResponse>(
        codec_, transportFactory_, key_, cacheName_, topologyId_, flags_),
      version(version_)
{}

VersionedOperationResponse RemoveIfUnmodifiedOperation::executeOperation(Transport& transport)
{
    // 1) write header
    hr_scoped_ptr<infinispan::hotrod::protocol::HeaderParams> params(
        &(AbstractKeyOperation<VersionedOperationResponse>::writeHeader(
            transport, REMOVE_IF_UNMODIFIED_REQUEST)));

    //2) write message body
    transport.writeArray(key);
    transport.writeLong(version);
    transport.flush();

    return AbstractKeyOperation<VersionedOperationResponse>::returnVersionedOperationResponse(transport, *params);

}

}}} /// namespace infinispan::hotrod::operations
