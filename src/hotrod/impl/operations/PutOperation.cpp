

#include "infinispan/hotrod/types.h"
#include "hotrod/impl/operations/PutOperation.h"

#include <sstream>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

PutOperation::PutOperation(
    const Codec&      codec_,
    HR_SHARED_PTR<transport::TransportFactory> transportFactory_,
    const hrbytes&    key_,
    const hrbytes&    cacheName_,
    IntWrapper&  topologyId_,
    uint32_t          flags_,
    const hrbytes&    value_,
    uint32_t          lifespan_,
    uint32_t          maxIdle_)
    : AbstractKeyValueOperation<hrbytes>(codec_, transportFactory_, key_,
        cacheName_, topologyId_, flags_, value_, lifespan_, maxIdle_)
{}

hrbytes PutOperation::executeOperation(Transport& transport) {
    TRACE("Executing Put(flags=%u, lifespan=%u, maxIdle=%u)", flags, lifespan, maxIdle);
    TRACEBYTES("key = ", key);
    TRACEBYTES("value = ", value);
    uint8_t status = sendPutOperation(transport, PUT_REQUEST, PUT_RESPONSE);
    if (status != NO_ERROR_STATUS) {
        std::ostringstream message;
        message << "Unexpected response status: " << status;
        throw InvalidResponseException(message.str());
    }
    return AbstractKeyValueOperation<hrbytes>::returnPossiblePrevValue(transport);
}


}}} /* namespace */
