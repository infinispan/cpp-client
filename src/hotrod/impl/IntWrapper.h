#ifndef INTWRAPPER_H_
#define INTWRAPPER_H_

#include "infinispan/hotrod/types.h"

namespace infinispan {
namespace hotrod {


class IntWrapper {
public:
  IntWrapper(uint32_t);
  virtual ~IntWrapper();
  uint32_t addAndGet(uint32_t delta);
  uint32_t incrementAndGet();
  uint32_t decrementAndGet();
  uint32_t get() const;
  void set(uint32_t);

private:
  uint32_t value;
};
}}
#endif /* INTWRAPPER_H_ */

