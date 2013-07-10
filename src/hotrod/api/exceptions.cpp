#include "infinispan/hotrod/exceptions.h"

namespace infinispan {
namespace hotrod {

Exception::Exception(const std::string& msg) throw() : message(msg) {}
Exception::~Exception() throw() {}
const char* Exception::what() const throw() { return message.c_str(); }

HotRodClientException::HotRodClientException(const std::string& msg) : Exception(msg) {}

TransportException::TransportException(const std::string& msg) : Exception(msg) {}

InvalidResponseException::InvalidResponseException(const std::string& msg) : Exception(msg) {}

}} /* namespace */
