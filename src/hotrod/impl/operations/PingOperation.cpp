

#include "hotrod/impl/operations/PingOperation.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/transport/Transport.h"

namespace infinispan {
namespace hotrod {

using namespace protocol;
using transport::Transport;

namespace operations {

PingOperation::PingOperation(const Codec& c, IntWrapper& id,
		Transport& t, const hrbytes& n)
    : HotRodOperation<PingResult>(c, 0, n, id), transport(t)
{}

PingOperation::PingOperation(const Codec& c, IntWrapper& id,
		Transport& t)
    : HotRodOperation<PingResult>(c, 0, hrbytes(), id), transport(t)
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
