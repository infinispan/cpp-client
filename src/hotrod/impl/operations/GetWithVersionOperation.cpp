#include "hotrod/impl/operations/GetWithVersionOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

GetWithVersionOperation::GetWithVersionOperation(
    const Codec&      codec_,
    std::shared_ptr<TransportFactory> transportFactory_,
    const std::vector<char>&    key_,
    const std::vector<char>&    cacheName_,
    IntWrapper&          topologyId_,
    uint32_t    flags_)
    : AbstractKeyOperation<VersionedValueImpl<std::vector<char>> >(
        codec_, transportFactory_, key_, cacheName_, topologyId_, flags_)
{}

VersionedValueImpl<std::vector<char>> GetWithVersionOperation::executeOperation(Transport& transport)
{
    TRACE("Execute GetWithVersion(flags=%u)", flags);
    TRACEBYTES("key = ", key);
    VersionedValueImpl<std::vector<char>> result;
    uint8_t status = sendKeyOperation(
        key, transport, GET_WITH_VERSION_REQUEST, GET_WITH_VERSION_RESPONSE);
    if (status == NO_ERROR_STATUS) {
        result.setVersion(transport.readLong());
        result.setValue(transport.readArray());
        TRACE("return version = %lld", result.version);
        TRACEBYTES("return value = ", result.getValue());
    } else {
        TRACE("Error status %u", status);
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations
