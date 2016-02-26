#include <hotrod/impl/Topology.h>

namespace infinispan {
namespace hotrod {

Topology::Topology(int initialValue) {
  id = initialValue;
}

Topology::~Topology() {
  id = 0;
}

int Topology::getId() const {
  return id;
}

void Topology::setId(int v) {
  id = v;
}

}
} //namespace
