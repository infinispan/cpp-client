#ifndef ISPN_HOTROD_EXCEPTIONS_H
#define ISPN_HOTROD_EXCEPTIONS_H

#include <exception>
#include <string>
#include <stdint.h>
#include "infinispan/hotrod/defs.h"
#include "infinispan/hotrod/portable.h"
#include "infinispan/hotrod/ImportExport.h"

namespace infinispan {
namespace hotrod {

class HR_EXTERN Exception : public std::exception
{
  public:
    explicit Exception(const std::string& message=std::string()) throw();
    virtual ~Exception() throw();
    virtual const char* what() const throw();

  protected:
    portable::string message;
};

/**
 * Main Exception class thrown to indicate exceptions using HotRod client
 *
 */
class HR_EXTERN HotRodClientException : public Exception
{
  public:
    explicit HotRodClientException(const std::string&);
    HotRodClientException(const std::string& message_, uint64_t message_id_, uint8_t status_);
    virtual ~HotRodClientException() throw();
    virtual const char* what() const throw();
  private:
    uint64_t message_id;
    uint8_t status;
};

/**
 * TransportException indicating TCP communication exceptions with
 * remote Hot Rod servers
 *
 */
class HR_EXTERN TransportException : public HotRodClientException
{
  public:
    TransportException(const std::string& host, int port, const std::string&);
    ~TransportException() throw();

    const std::string &getHost() const {
        if (hostPtr.get() == NULL) {
            const_cast<TransportException *>(this)->hostPtr.set(new std::string(host.c_string()), &deleteString);
        }
        return *(hostPtr.get());
    }
    const char *getHostCString() const;
    int getPort() const;
  private:
    const portable::string host;
    __pragma(warning(suppress:4251))
    portable::local_ptr<std::string> hostPtr;
    int port;

    static void deleteString(std::string *str) { delete str; }
};

/**
 * InvalidResponseException as it name implies indicates situations when some form of
 * invalid response is received from a remote Hot Rod server.
 *
 */
class HR_EXTERN InvalidResponseException : public HotRodClientException
{
  public:
    InvalidResponseException(const std::string&);
};

/**
 * RemoteNodeSuspectException as it name implies indicates situations when a remote
 * Hot Rod server is not reachable.
 *
 */
class HR_EXTERN RemoteNodeSuspectException : public HotRodClientException
{
  public:
    RemoteNodeSuspectException(const std::string&, uint64_t message_id, uint8_t status);
};

/**
 * InternalException
 *
 */
class HR_EXTERN InternalException : public HotRodClientException
{
  public:
    InternalException(const std::string&);
};

/**
 * RemoteCacheManagerNotStartedException is throw when invocations on a RemoteCache
 * are initiated while the underlying RemoteCacheManager has not been started previous to
 * those invocations.
 *
 */
class HR_EXTERN RemoteCacheManagerNotStartedException : public HotRodClientException
{
  public:
    RemoteCacheManagerNotStartedException(const std::string&);
};

/**
 * UnsupportedOperationException is thrown for uncompleted/planned Hot Rod client features.
 *
 */
struct HR_EXTERN UnsupportedOperationException : public HotRodClientException
{
	UnsupportedOperationException();
};

}} // namespace

#endif  /* ISPN_HOTROD_EXCEPTIONS_H */
