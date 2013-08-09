#ifndef ISPN_HOTROD_HANDLE_H
#define ISPN_HOTROD_HANDLE_H



#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/types.h"

namespace infinispan {
namespace hotrod {

template <class T> class Handle
{
  protected:
    typedef HR_SHARED_PTR<T> Impl;
    Handle(T* pimpl) : impl(pimpl){}
    Impl impl;
};

}} // namespace

#endif  /* ISPN_HOTROD_HANDLE_H */
