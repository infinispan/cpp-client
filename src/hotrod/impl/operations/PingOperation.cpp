#include "hotrod/impl/operations/PingOperation.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/transport/Transport.h"

namespace infinispan {
namespace hotrod {

using namespace protocol;
using transport::Transport;

namespace operations {

PingOperation::PingOperation(const Codec& c, uint32_t id,
		Transport& t, hrbytes& n)
    : HotRodOperation<PingResult>(c, n, id), transport(t)
{}

PingResult PingOperation::execute() {
    //HeaderParams params;
	hr_scoped_ptr<infinispan::hotrod::protocol::HeaderParams> params(&writeHeader(transport, HotRodConstants::PING_REQUEST));
	transport.flush();

    uint8_t respStatus = readHeaderAndValidate(transport, *params);
    if (respStatus == HotRodConstants::NO_ERROR_STATUS) {
    	return SUCCESS;
    } else {
    	return FAIL;
    }
}

}}} // namespace
