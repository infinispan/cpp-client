

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
    const std::vector<char>&    cacheName_,
    Topology&  topologyId_,
    uint32_t    flags_,
    int32_t  scope_)
    : RetryOnFailureOperation<std::set<std::vector<char>> >(
        codec_, transportFactory_, cacheName_, topologyId_, flags_), scope(scope_)
{}

std::set<std::vector<char>> BulkGetKeysOperation::executeOperation(Transport& transport)
{
    TRACE("Execute BulkGetKeys(flags=%u,scope=%d)", flags, scope);
    std::unique_ptr<HeaderParams> params(&(RetryOnFailureOperation<std::set<std::vector<char>> >::writeHeader(transport, BULK_GET_KEYS_REQUEST)));
    transport.writeVInt(scope);
    transport.flush();
    RetryOnFailureOperation<std::set<std::vector<char>> >::readHeaderAndValidate(transport, *params);
    std::set<std::vector<char>> result;
    while (transport.readByte()==1) {
        result.insert(transport.readArray());
    }
    if (logger.isTraceEnabled()) {
        for (std::set<std::vector<char>>::iterator it = result.begin(); it != result.end(); ++it) {
            TRACEBYTES("return key = ", *it);
        }
        if (result.size() == 0) {
            TRACE("No data in bulk");
        }
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations
