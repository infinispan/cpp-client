#ifndef ISPN_HOTROD_MARSHALLER_H
#define ISPN_HOTROD_MARSHALLER_H

#include "portable.h"

#include <vector>

namespace infinispan {
namespace hotrod {

// Todo: application buffer memory management; async readers/writers.  SSL implications?

template <class T> class Marshaller: public portable::counted_object
{
  public:
    virtual void marshall(const T&, std::vector<char>&) = 0;
    virtual T* unmarshall(const std::vector<char>&) = 0;

    virtual ~Marshaller() {}
    static void destroy(Marshaller<T> *marshaller) { delete marshaller; }
};

}} // namespace

#endif  /* ISPN_HOTROD_MARSHALLER_H */
