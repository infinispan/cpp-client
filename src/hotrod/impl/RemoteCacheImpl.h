#ifndef ISPN_HOTROD_REMOTECACHEIMPL_H
#define ISPN_HOTROD_REMOTECACHEIMPL_H

#include "infinispan/hotrod/types.h"
#include "infinispan/hotrod/ScopedBuffer.h"
#include "infinispan/hotrod/RemoteCacheBase.h"
#include "hotrod/impl/operations/OperationsFactory.h"

namespace infinispan {
namespace hotrod {

using namespace operations;

class RemoteCacheImpl
{
  public:
    RemoteCacheImpl(RemoteCacheBase& basexx);
    void get(const void* key, void *buf);
    void put(const void* key, const void* val);
    void ping();
  private:
    RemoteCacheBase& remoteCacheBase;
    // TODO: reference or pointer?
    infinispan::hotrod::operations::OperationsFactory* operationsFactory;
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHEIMPL_H */

