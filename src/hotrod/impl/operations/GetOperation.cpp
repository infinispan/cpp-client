#include "hotrod/sys/types.h"
#include "hotrod/impl/operations/GetOperation.h"
#include <cstring>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

GetOperation::GetOperation(
    const Codec&      codec_,
    TransportFactory& transportFactory_,
    const hrbytes&    key_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    const std::set<Flag>&    flags_)
    : AbstractKeyOperation<hrbytes>(
        codec_, transportFactory_, key_, cacheName_, topologyId_, flags_)
{}

hrbytes GetOperation::executeOperation(Transport* transport)
{
    hrbytes result;
    uint8_t status = sendKeyOperation(key,
        *transport, GET_REQUEST, GET_RESPONSE);
    if (status == KEY_DOES_NOT_EXIST_STATUS) {
        result.setSmart(0,0);
    } else {
        if (status == NO_ERROR_STATUS) {
            result = transport->readArray();
        }
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations
