#ifndef ISPN_HOTROD_MARSHALLER_H
#define ISPN_HOTROD_MARSHALLER_H

#include "portable.h"

#include <vector>

namespace infinispan {
namespace hotrod {

template <class T> class Marshaller
{
  public:
    virtual void marshall(const T&, std::vector<char>&) = 0;
    virtual T* unmarshall(const std::vector<char>&) = 0;

    virtual ~Marshaller() {}
    static void destroy(Marshaller<T> *marshaller) { delete marshaller; }
};

}} // namespace

#endif  /* ISPN_HOTROD_MARSHALLER_H */
