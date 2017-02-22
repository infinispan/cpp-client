#include "hotrod/impl/operations/BulkGetOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

BulkGetOperation::BulkGetOperation(
    const Codec&      codec_,
    std::shared_ptr<TransportFactory> transportFactory_,
    const std::vector<char>&    cacheName_,
    Topology&  topologyId_,
    uint32_t    flags_,
    int32_t  entryCount_)
    : RetryOnFailureOperation<std::map<std::vector<char>, std::vector<char>> >(
        codec_, transportFactory_, cacheName_, topologyId_, flags_), entryCount(entryCount_)
{}

std::map<std::vector<char>,std::vector<char>> BulkGetOperation::executeOperation(infinispan::hotrod::transport::Transport& transport)
{
    TRACE("Execute BulkGet(flags=%u, entryCount=%d)", flags, entryCount);
    std::unique_ptr<HeaderParams> params(RetryOnFailureOperation<std::map<std::vector<char>, std::vector<char>> >::writeHeader(transport, BULK_GET_REQUEST));
    transport.writeVInt(entryCount);
    transport.flush();
    RetryOnFailureOperation<std::map<std::vector<char>, std::vector<char>> >::readHeaderAndValidate(transport, *params);
    std::map<std::vector<char>,std::vector<char>> result;
    while (transport.readByte()==1) {
        std::vector<char> key = transport.readArray();
        std::vector<char> value = transport.readArray();
        result[key] = value;
    }
    if (logger.isTraceEnabled()) {
        for (std::map<std::vector<char>,std::vector<char>>::iterator it = result.begin(); it != result.end(); ++it) {
            TRACEBYTES("return key = ", it->first);
            TRACEBYTES("return value = ", it->second);
        }
        if (result.size() == 0) {
            TRACE("No data in bulk");
        }
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations
