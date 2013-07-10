#include "hotrod/sys/types.h"
#include "hotrod/impl/operations/PutOperation.h"
#include <cstring>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

PutOperation::PutOperation(
    const Codec&      codec_,
    TransportFactory& transportFactory_,
    const hrbytes&    key_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    const std::set<Flag>& flags_,
    const hrbytes&    value_,
    uint32_t          lifespan_,
    uint32_t          maxIdle_)
    : AbstractKeyValueOperation<hrbytes>(codec_, transportFactory_, key_,
        cacheName_, topologyId_, flags_, value_, lifespan_, maxIdle_)
{}

hrbytes PutOperation::executeOperation(Transport* transport) {
    uint8_t status = sendPutOperation(*transport, PUT_REQUEST, PUT_RESPONSE);
    if (status != NO_ERROR_STATUS) {
        throw HotRodClientException("PUT ERROR");
    }
    return AbstractKeyValueOperation<hrbytes>::returnPossiblePrevValue(*transport);
}


}}} /* namespace */
