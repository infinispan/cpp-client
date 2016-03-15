

#include "hotrod/impl/operations/PingOperation.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/transport/Transport.h"

namespace infinispan {
namespace hotrod {

using namespace protocol;
using transport::Transport;

namespace operations {

PingOperation::PingOperation(const Codec& c, Topology& id,
		Transport& t, const std::vector<char>& n)
    : HotRodOperation<PingResult>(c, 0, n, id), transport(t)
{}

PingOperation::PingOperation(const Codec& c, Topology& id,
		Transport& t)
    : HotRodOperation<PingResult>(c, 0, std::vector<char>(), id), transport(t)
{}

PingResult PingOperation::execute() {
	std::unique_ptr<infinispan::hotrod::protocol::HeaderParams> params(&writeHeader(transport, HotRodConstants::PING_REQUEST));
	transport.flush();

    uint8_t respStatus = readHeaderAndValidate(transport, *params);
    if (HotRodConstants::isSuccess(respStatus)) {
    	TRACE("Ping successful!");
    	if (HotRodConstants::hasCompatibility(respStatus)) {
    		WARN("Server has compatibility mode enabled. Inconsistency in hash calculation may occur");
    		return PingResult::SUCCESS_WITH_COMPAT;
    	}
    	else {
    		return PingResult::SUCCESS;
    	}
    } else {
    	TRACE("Ping failed!");
    	return FAIL;
    }
}

}}} // namespace
