#include "hotrod/sys/types.h"
#include "hotrod/impl/operations/OperationsFactory.h"
#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/operations/GetOperation.h"
#include "hotrod/impl/operations/PutOperation.h"
#include "hotrod/impl/operations/PingOperation.h"
#include "infinispan/hotrod/Flag.h"

#include <cstring>

namespace infinispan {
namespace hotrod {

using namespace protocol;
using namespace transport;

namespace operations {

OperationsFactory::OperationsFactory(
  TransportFactory& tf, const std::string& cn,
  uint32_t tid, bool frv, const Codec& c) :
	transportFactory(tf), codec(c), forceReturnValue(frv), topologyId(tid), flags()
{
  cacheNameBytes.reserve(cn.length());
  memcpy(cacheNameBytes.bytes(), cn.c_str(), cn.length());
}

PingOperation* OperationsFactory::newPingOperation(Transport& transport)
{
  return new PingOperation(codec, topologyId, transport, cacheNameBytes);
}

GetOperation* OperationsFactory::newGetKeyOperation(const hrbytes& key)
{
  return new GetOperation(
    codec, transportFactory, key, cacheNameBytes, topologyId, getFlags());
}

PutOperation* OperationsFactory::newPutKeyValueOperation(
  const hrbytes& key, const hrbytes& value,
  uint32_t lifespanSecs, uint32_t maxIdleSecs)
{
  return new PutOperation(
    codec, transportFactory, key, cacheNameBytes,
    topologyId, getFlags(), value, lifespanSecs, maxIdleSecs);
}

std::set<Flag> OperationsFactory::getFlags()
{
  std::set<Flag> result(flags);
  if (forceReturnValue) {
    result.insert(FORCE_RETURN_VALUE);
  }
  flags.clear();
  return result;
}

}}} // namespace infinispan::hotrod::operations
