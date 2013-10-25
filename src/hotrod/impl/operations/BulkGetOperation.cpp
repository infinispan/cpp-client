

#include "infinispan/hotrod/types.h"
#include "hotrod/impl/operations/BulkGetOperation.h"

#include <map>

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

BulkGetOperation::BulkGetOperation(
    const Codec&      codec_,
    HR_SHARED_PTR<TransportFactory> transportFactory_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    uint32_t    flags_,
    int32_t  entryCount_)
    : RetryOnFailureOperation<std::map<hrbytes, hrbytes> >(
        codec_, transportFactory_, cacheName_, topologyId_, flags_), entryCount(entryCount_)
{}

Transport& BulkGetOperation::getTransport(int /*retryCount*/)
{
        return RetryOnFailureOperation<std::map<hrbytes, hrbytes> >::transportFactory->getTransport();
}

std::map<hrbytes,hrbytes> BulkGetOperation::executeOperation(infinispan::hotrod::transport::Transport& transport)
{
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
    return result;
}

}}} /// namespace infinispan::hotrod::operations
