

#include "infinispan/hotrod/exceptions.h"

namespace infinispan {
namespace hotrod {

Exception::Exception(const std::string& msg) throw() : message(msg) {}
Exception::~Exception() throw() {}
const char* Exception::what() const throw() { return message.c_str(); }

HotRodClientException::HotRodClientException(const std::string& msg) : Exception(msg) {}

TransportException::TransportException(const std::string& host, int port, const std::string& msg)
: HotRodClientException(msg), serverAddress(host, port) {}

TransportException::~TransportException() throw() {}

const transport::InetSocketAddress& TransportException::getServerAddress() const { return serverAddress;}


InvalidResponseException::InvalidResponseException(const std::string& msg) : HotRodClientException(msg) {}

RemoteNodeSuspectException::RemoteNodeSuspectException(const std::string& msg) : HotRodClientException(msg) {}

InternalException::InternalException(const std::string& msg) : HotRodClientException(msg) {}

}} /* namespace */
