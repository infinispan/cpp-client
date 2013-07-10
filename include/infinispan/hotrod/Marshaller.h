#ifndef ISPN_HOTROD_MARSHALLER_H
#define ISPN_HOTROD_MARSHALLER_H

#include "infinispan/hotrod/ScopedBuffer.h"

namespace infinispan {
namespace hotrod {

class ScopedBuffer;

// Todo: application buffer memory management; async readers/writers.  SSL implications?

template <class T> class Marshaller
{
  public:
    virtual void marshall(const T&, ScopedBuffer&) = 0;
    virtual T* unmarshall(const ScopedBuffer&) = 0;
};

}} // namespace

#endif  /* ISPN_HOTROD_MARSHALLER_H */
