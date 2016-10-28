#include "infinispan/hotrod/exceptions.h"

#include <sstream>

namespace infinispan {
namespace hotrod {

Exception::Exception(const std::string& msg) throw() : message(msg) {}
Exception::~Exception() throw() {}
const char* Exception::what() const throw() { return message.c_string(); }

HotRodClientException::HotRodClientException(const std::string& msg) : Exception(msg), message_id(0), status(0) {}

HotRodClientException::HotRodClientException(const std::string& msg, uint64_t message_id_, uint8_t status_) : message_id(message_id_), status(status_) {
    std::stringstream s;
    if (message_id > 0) {
        s << "Request for message id[" << message_id << "]";
    }
    if (status > 0) {
        s << " returned " << status;
    }
    s << msg;
    message = s.str();
}

HotRodClientException::~HotRodClientException() throw() {}

const char* HotRodClientException::what() const throw() {
    return message.c_string();
}

TransportException::TransportException(const std::string& h, int p, const std::string& msg, int errnum)
: HotRodClientException(msg), host(h), port(p), errnum(errnum) {}

TransportException::~TransportException() throw() {}

const char *TransportException::getHostCString() const { return host.c_string(); }
int TransportException::getPort() const { return port;}

InvalidResponseException::InvalidResponseException(const std::string& msg) : HotRodClientException(msg) {}

RemoteNodeSuspectException::RemoteNodeSuspectException(const std::string& msg, uint64_t message_id_, uint8_t status_) : HotRodClientException(msg, message_id_, status_) {}

InternalException::InternalException(const std::string& msg) : HotRodClientException(msg) {}

RemoteCacheManagerNotStartedException::RemoteCacheManagerNotStartedException(const std::string& msg) : HotRodClientException(msg) {}

UnsupportedOperationException::UnsupportedOperationException() : HotRodClientException("Unsupported operation.") {}
}} /* namespace */
