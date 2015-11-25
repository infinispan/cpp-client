

#include "hotrod/impl/operations/BulkGetKeysOperation.h"

#include <set>

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

BulkGetKeysOperation::BulkGetKeysOperation(
    const Codec&      codec_,
    std::shared_ptr<TransportFactory> transportFactory_,
    const hrbytes&    cacheName_,
    IntWrapper&  topologyId_,
    uint32_t    flags_,
    int32_t  scope_)
    : RetryOnFailureOperation<std::set<hrbytes> >(
        codec_, transportFactory_, cacheName_, topologyId_, flags_), scope(scope_)
{}

Transport& BulkGetKeysOperation::getTransport(int /*retryCount*/)
{
        return RetryOnFailureOperation<std::set<hrbytes> >::transportFactory->getTransport(cacheName);
}

std::set<hrbytes> BulkGetKeysOperation::executeOperation(Transport& transport)
{
    TRACE("Execute BulkGetKeys(flags=%u,scope=%d)", flags, scope);
    hr_scoped_ptr<HeaderParams> params(&(RetryOnFailureOperation<std::set<hrbytes> >::writeHeader(transport, BULK_GET_KEYS_REQUEST)));
    transport.writeVInt(scope);
    transport.flush();
    RetryOnFailureOperation<std::set<hrbytes> >::readHeaderAndValidate(transport, *params);
    std::set<hrbytes> result;
    while (transport.readByte()==1) {
        result.insert(transport.readArray());
    }
    if (logger.isTraceEnabled()) {
        for (std::set<hrbytes>::iterator it = result.begin(); it != result.end(); ++it) {
            TRACEBYTES("return key = ", *it);
        }
        if (result.size() == 0) {
            TRACE("No data in bulk");
        }
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations
