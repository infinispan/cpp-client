#ifndef ISPN_HOTROD_EXCEPTIONS_H
#define ISPN_HOTROD_EXCEPTIONS_H



#include "infinispan/hotrod/ImportExport.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"
#include <exception>
#include <string>

namespace infinispan {
namespace hotrod {

class HR_EXTERN Exception : public std::exception
{
  public:
    explicit Exception(const std::string& message=std::string()) throw();
    virtual ~Exception() throw();
    virtual const char* what() const throw();

  private:
    const std::string message;
};

// Matching the names of the Java exceptions where possible, adding if
// necessary.

struct HR_EXTERN HotRodClientException : public Exception
{
    HotRodClientException(const std::string&);
    //virtual ~HotRodClientException() throw();
};

struct HR_EXTERN TransportException : public HotRodClientException
{
    transport::InetSocketAddress serverAddress;
    TransportException(const std::string& host, int port,
    		const std::string&);
    ~TransportException() throw();

    const transport::InetSocketAddress& getServerAddress() const;
};

struct HR_EXTERN InvalidResponseException : public HotRodClientException
{
	InvalidResponseException(const std::string&);
};

struct HR_EXTERN RemoteNodeSuspectException : public HotRodClientException
{
	RemoteNodeSuspectException(const std::string&);
};

struct HR_EXTERN InternalException : public HotRodClientException
{
	InternalException(const std::string&);
};

}} // namespace

#endif  /* ISPN_HOTROD_EXCEPTIONS_H */
