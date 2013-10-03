

#include "infinispan/hotrod/exceptions.h"

namespace infinispan {
namespace hotrod {

Exception::Exception(const std::string& msg) throw() : message(msg) {}
Exception::~Exception() throw() {}
const char* Exception::what() const throw() { return message.c_str(); }

HotRodClientException::HotRodClientException(const std::string& msg) : Exception(msg) {}

TransportException::TransportException(const std::string& h, int p, const std::string& msg)
: HotRodClientException(msg), host(h), port(p) {}

TransportException::~TransportException() throw() {}

const std::string& TransportException::getHost() const { return host;}
int TransportException::getPort() const { return port;}


//const transport::InetSocketAddress& TransportException::getServerAddress() const { return serverAddress;}


InvalidResponseException::InvalidResponseException(const std::string& msg) : HotRodClientException(msg) {}

RemoteNodeSuspectException::RemoteNodeSuspectException(const std::string& msg) : HotRodClientException(msg) {}

InternalException::InternalException(const std::string& msg) : HotRodClientException(msg) {}

RemoteCacheManagerNotStartedException::RemoteCacheManagerNotStartedException(const std::string& msg) : HotRodClientException(msg) {}

RemoteCacheNotExistException::RemoteCacheNotExistException(const std::string& msg) : HotRodClientException(msg) {}

}} /* namespace */
