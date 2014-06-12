#ifndef ISPN_HOTROD_MARSHALLER_H
#define ISPN_HOTROD_MARSHALLER_H

#include "portable.h"
#include "infinispan/hotrod/ScopedBuffer.h"

namespace infinispan {
namespace hotrod {

class ScopedBuffer;

// Todo: application buffer memory management; async readers/writers.  SSL implications?

template <class T> class Marshaller: public portable::counted_object
{
  public:
    virtual void marshall(const T&, ScopedBuffer&) = 0;
    virtual T* unmarshall(const ScopedBuffer&) = 0;

    virtual ~Marshaller() {}
    static void destroy(Marshaller<T> *marshaller) { delete marshaller; }
};

}} // namespace

#endif  /* ISPN_HOTROD_MARSHALLER_H */
