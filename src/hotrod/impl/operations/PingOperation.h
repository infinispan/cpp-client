#ifndef ISPN_HOTROD_OPERATIONS_PINGOPERATION_H
#define ISPN_HOTROD_OPERATIONS_PINGOPERATION_H



#include "hotrod/impl/operations/HotRodOperation.h"

namespace infinispan {
namespace hotrod {
class hrbytes;
class IntWrapper;

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
    PingOperation(
        const protocol::Codec& codec,
        IntWrapper& topologyId,
        transport::Transport& transport);

  private:
    PingOperation(
        const protocol::Codec& codec,
        IntWrapper& topologyId,
        transport::Transport& transport,
        const hrbytes& cacheName);

    transport::Transport& transport;

  friend class OperationsFactory;
  friend class FaultTolerantPingOperation;
};

}}} // namespace infinispan::hotrod::operations

#endif /* ISPN_HOTROD_OPERATIONS_PINGOPERATION_H */
