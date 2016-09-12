#ifndef ISPN_HOTROD_FLAG_H
#define ISPN_HOTROD_FLAG_H

namespace infinispan {
namespace hotrod {

enum Flag
{
  FORCE_RETURN_VALUE = 0x01,
  DEFAULT_LIFESPAN   = 0x02,
  DEFAULT_MAXIDLE    = 0x04
};

enum ClusterStatus { SWITCHED, NOT_SWITCHED, ALREADY_SWITCHED };

}} // namespace

#endif /* ISPN_HOTROD_FLAG_H */
