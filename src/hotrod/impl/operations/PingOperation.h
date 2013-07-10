#ifndef ISPN_HOTROD_OPERATIONS_PINGOPERATION_H
#define ISPN_HOTROD_OPERATIONS_PINGOPERATION_H

#include "hotrod/impl/operations/HotRodOperation.h"

#include <stdint.h>

namespace infinispan {
namespace hotrod {
class hrbytes;

namespace operations {

enum PingResult
{
  // Success if the ping request was responded correctly
  SUCCESS,
  // When the ping request fails due to non-existing cache
  CACHE_DOES_NOT_EXIST,
  // For any other type of failures
  FAIL
};

class PingOperation : public HotRodOperation<PingResult>
{
  public:
    PingResult execute();

  private:
    PingOperation(const infinispan::hotrod::protocol::Codec& codec,
        	      uint32_t topologyId,
                  infinispan::hotrod::transport::Transport& transport,
                  hrbytes& cacheName);

    infinispan::hotrod::transport::Transport& transport;

  friend class OperationsFactory;
};

}}} // namespace infinispan::hotrod::operations

#endif /* ISPN_HOTROD_OPERATIONS_PINGOPERATION_H */
