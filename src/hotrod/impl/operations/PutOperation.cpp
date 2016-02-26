#include "hotrod/impl/operations/PutOperation.h"

#include <sstream>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

PutOperation::PutOperation(
    const Codec&      codec_,
    std::shared_ptr<transport::TransportFactory> transportFactory_,
    const std::vector<char>&    key_,
    const std::vector<char>&    cacheName_,
    Topology&  topologyId_,
    uint32_t          flags_,
    const std::vector<char>&    value_,
    uint32_t          lifespan_,
    uint32_t          maxIdle_)
    : AbstractKeyValueOperation<std::vector<char>>(codec_, transportFactory_, key_,
        cacheName_, topologyId_, flags_, value_, lifespan_, maxIdle_)
{}

std::vector<char> PutOperation::executeOperation(Transport& transport) {
    TRACE("Executing Put(flags=%u, lifespan=%u, maxIdle=%u)", flags, lifespan, maxIdle);
    TRACEBYTES("key = ", key);
    TRACEBYTES("value = ", value);
    uint8_t status = sendPutOperation(transport, PUT_REQUEST, PUT_RESPONSE);
    if (!HotRodConstants::isSuccess(status)) {
        std::ostringstream message;
        message << "Unexpected response status: " << status;
        throw InvalidResponseException(message.str());
    }
    return AbstractKeyValueOperation<std::vector<char>>::returnPossiblePrevValue(transport, status);
}


}}} /* namespace */
