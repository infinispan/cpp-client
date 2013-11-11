#include "hotrod/impl/IntWrapper.h"

namespace infinispan {
namespace hotrod {

IntWrapper::IntWrapper(uint32_t initialValue) {
  value = initialValue;
}

IntWrapper::~IntWrapper() {
  value = 0;
}

uint32_t IntWrapper::addAndGet(uint32_t delta) {
  value += delta;
  return value;
}

uint32_t IntWrapper::incrementAndGet() {
  value += 1;
  return value;
}

uint32_t IntWrapper::decrementAndGet() {
  value -= 1;
  return value;
}

uint32_t IntWrapper::get() const {
  return value;
}

void IntWrapper::set(uint32_t v) {
  value = v;
}

}
} //namespace
