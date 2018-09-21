#include "hotrod/impl/operations/GetAllOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

GetAllOperation::GetAllOperation(const Codec& _codec, std::shared_ptr<transport::TransportFactory> _transportFactory, const std::set<std::vector<char>>& keySet,
	    const std::vector<char>& _cacheName, Topology& _topologyId, uint32_t _flags, EntryMediaTypes* df)
        : RetryOnFailureOperation<std::map<std::vector<char>, std::vector<char>>>(_codec, _transportFactory, _cacheName, _topologyId, _flags, df), keySet(keySet) {}


std::map<std::vector<char>, std::vector<char>> GetAllOperation::executeOperation(infinispan::hotrod::transport::Transport& transport)
{
	std::map<std::vector<char>, std::vector<char>> result;
	TRACE("Executing GetAll(flags=%u)", flags);
	std::unique_ptr<protocol::HeaderParams> params(this->writeHeader(transport, GET_ALL_REQUEST));
	transport.writeVInt(keySet.size());
	for (auto &item : keySet)
	{
		transport.writeArray(item);
		transport.flush();
	}
	uint8_t status = readHeaderAndValidate(transport, *params);
	if (status == NO_ERROR_STATUS) {
		uint32_t count = transport.readVInt();
		for (uint32_t i = 0; i < count; i++)
				{
			std::vector<char> k = transport.readArray();
			std::vector<char> v = transport.readArray();
			result[k] = v;
		}
	} else {
		TRACE("Error status %u", status);
	}
return result;
}

}}}
