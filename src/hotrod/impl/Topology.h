#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

#include "infinispan/hotrod/defs.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
namespace infinispan {
namespace hotrod {


class Topology {
public:
  Topology() : id(protocol::HotRodConstants::DEFAULT_CACHE_TOPOLOGY) {};
  Topology(int);
  virtual ~Topology();
  int getId() const;
  void setId(int);

private:
  int id;
};
}}
#endif /* TOPOLOGY_H_ */

