#ifndef ISPN_HOTROD_EXCEPTIONS_H
#define ISPN_HOTROD_EXCEPTIONS_H

#include "infinispan/hotrod/ImportExport.h"
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
};

struct HR_EXTERN TransportException : public Exception
{
    TransportException(const std::string&);
};

struct HR_EXTERN InvalidResponseException : public Exception
{
	InvalidResponseException(const std::string&);
};

}} // namespace

#endif  /* ISPN_HOTROD_EXCEPTIONS_H */
