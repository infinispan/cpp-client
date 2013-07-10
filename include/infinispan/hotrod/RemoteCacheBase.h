#ifndef ISPN_HOTROD_REMOTECACHEBASE_H
#define ISPN_HOTROD_REMOTECACHEBASE_H

#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/Handle.h"

namespace infinispan {
namespace hotrod {

class RemoteCacheImpl;
typedef void (*MarshallHelperFn) (void*, const void *, void*);

class HR_EXTERN RemoteCacheBase
    : public infinispan::hotrod::Handle<RemoteCacheImpl>
{
 public:
    void base_get(const void *key, void *rbuf);
    void base_put(const void *key, const void *value);
    void base_ping();
  protected:
    RemoteCacheBase();
    void setMarshallers(void* rc, MarshallHelperFn kf, MarshallHelperFn vf);

  private:
    void *remoteCachePtr;
    MarshallHelperFn baseKeyMarshallFn;
    MarshallHelperFn baseValueMarshallFn;
    void baseKeyMarshall(const void* k, void *buf);
    void baseValueMarshall(const void* v, void *buf);

  friend class RemoteCacheManagerImpl;
  friend class RemoteCacheImpl;
};

}} // namespace

#endif  /* ISPN_HOTROD_REMOTECACHEBASE_H */
