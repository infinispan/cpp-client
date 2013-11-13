#ifndef ISPN_HOTROD_EXCEPTIONS_H
#define ISPN_HOTROD_EXCEPTIONS_H

#include <exception>
#include <string>
#include <stdint.h>
#include "infinispan/hotrod/ImportExport.h"

namespace infinispan {
namespace hotrod {

class HR_EXTERN Exception : public std::exception
{
  public:
    explicit Exception(const std::string& message=std::string()) throw();
    virtual ~Exception() throw();
    virtual const char* what() const throw();

  private:
    std::string message;
};

/*
 * FIXME: We should really extend Exception, but apparently g++ is losing specific typeinfo across the shared library boundary
 */
class HR_EXTERN HotRodClientException : public std::exception
{
  public:
    explicit HotRodClientException(const std::string&);
    HotRodClientException(const std::string& message_, uint64_t message_id_, uint8_t status_);
    virtual ~HotRodClientException() throw();
    virtual const char* what() const throw();
  private:
    std::string message;
    uint64_t message_id;
    uint8_t status;
};

class HR_EXTERN TransportException : public HotRodClientException
{
  public:
    TransportException(const std::string& host, int port, const std::string&);
    ~TransportException() throw();

    const std::string& getHost() const;
    int getPort() const;
  private:
    const std::string host;
    int port;
};

class HR_EXTERN InvalidResponseException : public HotRodClientException
{
  public:
    InvalidResponseException(const std::string&);
};

class HR_EXTERN RemoteNodeSuspectException : public HotRodClientException
{
  public:
    RemoteNodeSuspectException(const std::string&, uint64_t message_id, uint8_t status);
};

class HR_EXTERN InternalException : public HotRodClientException
{
  public:
    InternalException(const std::string&);
};

class HR_EXTERN RemoteCacheManagerNotStartedException : public HotRodClientException
{
  public:
    RemoteCacheManagerNotStartedException(const std::string&);
};

struct HR_EXTERN UnsupportedOperationException : public HotRodClientException
{
	UnsupportedOperationException();
};

}} // namespace

#endif  /* ISPN_HOTROD_EXCEPTIONS_H */
