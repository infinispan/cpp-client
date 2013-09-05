#ifndef ISPN_HOTROD_RUNNABLE_H
#define ISPN_HOTROD_RUNNABLE_H

#include "infinispan/hotrod/ImportExport.h"

namespace infinispan {
namespace hotrod {
namespace sys {

class HR_EXTERN Runnable
{
  public:
    virtual ~Runnable();
    virtual void run() = 0;
};

}}}
#endif  /* ISPN_HOTROD_RUNNABLE_H */
