#include <hotrod/impl/Topology.h>
#include "hotrod/impl/operations/OperationsFactory.h"
#include "hotrod/impl/protocol/CodecFactory.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/operations/GetOperation.h"
#include "hotrod/impl/operations/PutOperation.h"
#include "hotrod/impl/operations/PutIfAbsentOperation.h"
#include "hotrod/impl/operations/ReplaceOperation.h"
#include "hotrod/impl/operations/RemoveOperation.h"
#include "hotrod/impl/operations/ContainsKeyOperation.h"
#include "hotrod/impl/operations/PingOperation.h"
#include "hotrod/impl/operations/ReplaceIfUnmodifiedOperation.h"
#include "hotrod/impl/operations/RemoveIfUnmodifiedOperation.h"
#include "hotrod/impl/operations/GetWithMetadataOperation.h"
#include "hotrod/impl/operations/GetWithVersionOperation.h"
#include "hotrod/impl/operations/BulkGetOperation.h"
#include "hotrod/impl/operations/BulkGetKeysOperation.h"
#include "hotrod/impl/operations/StatsOperation.h"
#include "hotrod/impl/operations/ClearOperation.h"
#include "hotrod/impl/operations/SizeOperation.h"
#include "hotrod/impl/operations/FaultTolerantPingOperation.h"
#include "hotrod/impl/operations/ExecuteCmdOperation.h"
#include "hotrod/impl/operations/QueryOperation.h"
#include "infinispan/hotrod/Flag.h"

#include <cstring>

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace protocol;
using namespace transport;

OperationsFactory::OperationsFactory(
    std::shared_ptr<infinispan::hotrod::transport::TransportFactory> tf, const std::string& cn,
    bool frv, const Codec& c) :
        transportFactory(tf), codec(c), forceReturnValue(frv), flags(), cacheNameBytes(cn.begin(),cn.end())
{
	if (transportFactory)
	{
	  topologyId=transportFactory->createTopologyId(cacheNameBytes);
	}
}

PingOperation* OperationsFactory::newPingOperation(Transport& transport)
{
    return new PingOperation(codec, topologyId, transport, cacheNameBytes);
}

GetOperation* OperationsFactory::newGetKeyOperation(const std::vector<char>& key)
{
    return new GetOperation(
        codec, transportFactory, key, cacheNameBytes, topologyId, getFlags());
}

PutOperation* OperationsFactory::newPutKeyValueOperation(
    const std::vector<char>& key, const std::vector<char>& value,
    uint32_t lifespanSecs, uint32_t maxIdleSecs)
{
    return new PutOperation(
        codec, transportFactory, key, cacheNameBytes,
        topologyId, getFlags(), value, lifespanSecs, maxIdleSecs);
}

PutIfAbsentOperation* OperationsFactory::newPutIfAbsentOperation(
    const std::vector<char>& key, const std::vector<char>& value,
    uint32_t lifespanSecs, uint32_t maxIdleSecs)
{
    return new PutIfAbsentOperation(
        codec, transportFactory, key, cacheNameBytes,
        topologyId, getFlags(), value, lifespanSecs, maxIdleSecs);
}

ReplaceOperation* OperationsFactory::newReplaceOperation(
    const std::vector<char>& key, const std::vector<char>& value,
    uint32_t lifespanSecs, uint32_t maxIdleSecs)
{
    return new ReplaceOperation(
        codec, transportFactory, key, cacheNameBytes,
        topologyId, getFlags(), value, lifespanSecs, maxIdleSecs);
}

RemoveOperation* OperationsFactory::newRemoveOperation(const std::vector<char>& key) {
    return new RemoveOperation(
        codec, transportFactory, key, cacheNameBytes, topologyId, getFlags());
}

ContainsKeyOperation* OperationsFactory::newContainsKeyOperation(const std::vector<char>& key) {
    return new ContainsKeyOperation(
        codec, transportFactory, key, cacheNameBytes, topologyId, getFlags());
}

ReplaceIfUnmodifiedOperation* OperationsFactory::newReplaceIfUnmodifiedOperation(
    const std::vector<char>& key, const std::vector<char>& value,
    uint32_t lifespanSecs, uint32_t maxIdleSecs, int64_t version)
{
    return new ReplaceIfUnmodifiedOperation(
        codec, transportFactory, key, cacheNameBytes,
        topologyId, getFlags(), value, lifespanSecs, maxIdleSecs, version);
}

RemoveIfUnmodifiedOperation* OperationsFactory::newRemoveIfUnmodifiedOperation(
    const std::vector<char>& key, int64_t version)
{
    return new RemoveIfUnmodifiedOperation(
        codec, transportFactory, key, cacheNameBytes, topologyId, getFlags(), version);
}

GetWithMetadataOperation* OperationsFactory::newGetWithMetadataOperation(const std::vector<char>& key) {
    return new GetWithMetadataOperation(
        codec, transportFactory, key, cacheNameBytes, topologyId, getFlags());
}

GetWithVersionOperation* OperationsFactory::newGetWithVersionOperation(const std::vector<char>& key) {
    return new GetWithVersionOperation(
        codec, transportFactory, key, cacheNameBytes, topologyId, getFlags());
}

BulkGetOperation* OperationsFactory::newBulkGetOperation(int size) {
    return new BulkGetOperation(
        codec, transportFactory, cacheNameBytes, topologyId, getFlags(), size);
}

BulkGetKeysOperation* OperationsFactory::newBulkGetKeysOperation(int scope) {
    return new BulkGetKeysOperation(
        codec, transportFactory, cacheNameBytes, topologyId, getFlags(), scope);
}

StatsOperation* OperationsFactory::newStatsOperation() {
    return new StatsOperation(
        codec, transportFactory, cacheNameBytes, topologyId, getFlags());
}

ClearOperation* OperationsFactory::newClearOperation() {
    return new ClearOperation(
        codec, transportFactory, cacheNameBytes, topologyId, getFlags());
}

SizeOperation* OperationsFactory::newSizeOperation() {
    return new SizeOperation(
        codec, transportFactory, cacheNameBytes, topologyId, getFlags());
}

FaultTolerantPingOperation* OperationsFactory::newFaultTolerantPingOperation() {
    return new FaultTolerantPingOperation(
        codec, transportFactory, cacheNameBytes, topologyId, getFlags());
}

ExecuteCmdOperation* OperationsFactory::newExecuteCmdOperation(
    const std::vector<char>& cmdName, const std::map<std::vector<char>,std::vector<char>>& values)
{
    return new ExecuteCmdOperation(
        codec, transportFactory, cacheNameBytes,
        topologyId, getFlags(), cmdName, values);
}

QueryOperation* OperationsFactory::newQueryOperation(const QueryRequest& qr)
{
	return new QueryOperation(codec, transportFactory, cacheNameBytes,
	        topologyId, getFlags(), qr);
}

uint32_t OperationsFactory::getFlags() {
    uint32_t result = flags;
    if (forceReturnValue) {
        result |= FORCE_RETURN_VALUE;
    }
    flags = 0;
    return result;
}

void OperationsFactory::addFlags(uint32_t f) {
    flags |= f;
}

void OperationsFactory::setFlags(uint32_t f) {
    flags = f;
}

CacheTopologyInfo infinispan::hotrod::operations::OperationsFactory::getCacheTopologyInfo() {
	    	return transportFactory->getCacheTopologyInfo(cacheNameBytes);
	    }

}
}
} // namespace infinispan::hotrod::operations


