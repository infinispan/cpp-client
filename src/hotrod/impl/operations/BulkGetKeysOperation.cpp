

#include "infinispan/hotrod/types.h"
#include "hotrod/impl/operations/BulkGetKeysOperation.h"

#include <set>

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

BulkGetKeysOperation::BulkGetKeysOperation(
    const Codec&      codec_,
    HR_SHARED_PTR<TransportFactory> transportFactory_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    uint32_t    flags_,
    int32_t  scope_)
    : RetryOnFailureOperation<std::set<hrbytes> >(
        codec_, transportFactory_, cacheName_, topologyId_, flags_), scope(scope_)
{}

Transport& BulkGetKeysOperation::getTransport(int /*retryCount*/)
{
        return RetryOnFailureOperation<std::set<hrbytes> >::transportFactory->getTransport();
}

std::set<hrbytes> BulkGetKeysOperation::executeOperation(Transport& transport)
{
    hr_scoped_ptr<HeaderParams> params(&(RetryOnFailureOperation<std::set<hrbytes> >::writeHeader(transport, BULK_GET_KEYS_REQUEST)));
    transport.writeVInt(scope);
    transport.flush();
    RetryOnFailureOperation<std::set<hrbytes> >::readHeaderAndValidate(transport, *params);
    std::set<hrbytes> result;
    while (transport.readByte()==1) {
        result.insert(transport.readArray());
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations
