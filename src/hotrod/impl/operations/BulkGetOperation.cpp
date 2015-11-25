#include "hotrod/impl/operations/BulkGetOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

BulkGetOperation::BulkGetOperation(
    const Codec&      codec_,
    std::shared_ptr<TransportFactory> transportFactory_,
    const hrbytes&    cacheName_,
    IntWrapper&  topologyId_,
    uint32_t    flags_,
    int32_t  entryCount_)
    : RetryOnFailureOperation<std::map<hrbytes, hrbytes> >(
        codec_, transportFactory_, cacheName_, topologyId_, flags_), entryCount(entryCount_)
{}

Transport& BulkGetOperation::getTransport(int /*retryCount*/)
{
        return RetryOnFailureOperation<std::map<hrbytes, hrbytes> >::transportFactory->getTransport(cacheName);
}

std::map<hrbytes,hrbytes> BulkGetOperation::executeOperation(infinispan::hotrod::transport::Transport& transport)
{
    TRACE("Execute BulkGet(flags=%u, entryCount=%d)", flags, entryCount);
    hr_scoped_ptr<HeaderParams> params(&(RetryOnFailureOperation<std::map<hrbytes, hrbytes> >::writeHeader(transport, BULK_GET_REQUEST)));
    transport.writeVInt(entryCount);
    transport.flush();
    RetryOnFailureOperation<std::map<hrbytes, hrbytes> >::readHeaderAndValidate(transport, *params);
    std::map<hrbytes,hrbytes> result;
    while (transport.readByte()==1) {
        hrbytes key = transport.readArray();
        hrbytes value = transport.readArray();
        result[key] = value;
    }
    if (logger.isTraceEnabled()) {
        for (std::map<hrbytes,hrbytes>::iterator it = result.begin(); it != result.end(); ++it) {
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
