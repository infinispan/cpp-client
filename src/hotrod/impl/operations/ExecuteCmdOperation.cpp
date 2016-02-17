#include "hotrod/impl/operations/ExecuteCmdOperation.h"

#include <sstream>

namespace infinispan {
namespace hotrod {
namespace operations {

using infinispan::hotrod::protocol::Codec;
using namespace infinispan::hotrod::transport;

ExecuteCmdOperation::ExecuteCmdOperation(
    const Codec&      codec_,
    std::shared_ptr<transport::TransportFactory> transportFactory_,
    const std::vector<char>&    cacheName_,
    IntWrapper&  topologyId_,
    uint32_t          flags_,
    const std::vector<char>&    cmdName_,
	const portable::map<std::vector<char>,std::vector<char>>&   cmdArgs_)
	: RetryOnFailureOperation<std::vector<char>>(
        codec_, transportFactory_, cacheName_, topologyId_, flags_), cmdName(cmdName_), cmdArgs(cmdArgs_)
{}

std::vector<char> ExecuteCmdOperation::executeOperation(Transport& transport) {
    return sendExecuteOperation(transport, EXEC_REQUEST, EXEC_RESPONSE);

}

std::vector<char> ExecuteCmdOperation::sendExecuteOperation(
    transport::Transport&     transport,
    uint8_t                                       opCode,
    uint8_t                                       /*opRespCode*/)
{
    // 1) write header
    hr_scoped_ptr<protocol::HeaderParams> params(&(this->writeHeader(transport, opCode)));

    // 2) write key and value
    transport.writeArray(this->cmdName);
    transport.writeVInt(this->cmdArgs.size());

    for (int i=this->cmdArgs.size()-1; i>=0; i--)
    {
        std::vector<char> nameBuf, valBuf;
    	transport.writeArray(this->cmdArgs.data()[i].key);
    	transport.writeArray(this->cmdArgs.data()[i].value);
    }
    transport.flush();

    // 3) now read header
    RetryOnFailureOperation<std::vector<char>>::readHeaderAndValidate(transport, *params);
       return transport.readArray();

}

Transport& ExecuteCmdOperation::getTransport(int /*retryCount*/)
{
        return RetryOnFailureOperation<std::vector<char>>::transportFactory->getTransport(cacheName);
}


}}} /* namespace */
