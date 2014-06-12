#include "hotrod/impl/operations/StatsOperation.h"

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

StatsOperation::StatsOperation(
    const Codec&      codec_,
    HR_SHARED_PTR<transport::TransportFactory> transportFactory_,
    const hrbytes&    cacheName_,
    IntWrapper&  topologyId_,
    uint32_t    flags_)
    : RetryOnFailureOperation<std::map<std::string, std::string> >(
        codec_, transportFactory_, cacheName_, topologyId_, flags_)
{}

Transport& StatsOperation::getTransport(int /*retryCount*/)
{
        return RetryOnFailureOperation<std::map<std::string, std::string> >::transportFactory->getTransport();
}

std::map<std::string, std::string> StatsOperation::executeOperation(Transport& transport)
{
    TRACE("Executing Stats");
    hr_scoped_ptr<HeaderParams> params(&(RetryOnFailureOperation<std::map<std::string, std::string> >::writeHeader(transport, STATS_REQUEST)));
    transport.flush();
    RetryOnFailureOperation<std::map<std::string, std::string> >::readHeaderAndValidate(transport, *params);

    int nrOfStats = transport.readVInt();
    TRACE("Stats returning map of %d entries:", nrOfStats);
    std::map<std::string, std::string> result;
    for (int i = 0; i < nrOfStats; i++) {
        std::string statName = transport.readString();
        std::string statValue = transport.readString();
        result[statName] = statValue;
        TRACE("%s -> %s", statName.c_str(), statValue.c_str());
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations
