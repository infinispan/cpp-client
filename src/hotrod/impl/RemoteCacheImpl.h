#ifndef ISPN_HOTROD_REMOTECACHEIMPL_H
#define ISPN_HOTROD_REMOTECACHEIMPL_H

#include "infinispan/hotrod/RemoteCacheBase.h"
#include "hotrod/impl/MetadataValueImpl.h"
#include "hotrod/impl/VersionedValueImpl.h"
#include "hotrod/impl/operations/PingOperation.h"
#include "infinispan/hotrod/Query.h"

using namespace org::infinispan::query::remote::client;

namespace infinispan {
namespace hotrod {

namespace operations {
class OperationsFactory;
}

class RemoteCacheManagerImpl;

class RemoteCacheImpl
{
public:
    RemoteCacheImpl(RemoteCacheManagerImpl& rcm, const std::string& name);
    virtual void *get(RemoteCacheBase& rcb, const void* key);
    std::map<std::vector<char>,std::vector<char>> getAll(const std::set<std::vector<char>>& keySet);
    virtual void *put(RemoteCacheBase& rcb, const void *key, const void* val, uint64_t life, uint64_t idle);
    void *putIfAbsent(RemoteCacheBase& rcb, const void *key, const void* val, uint64_t life, uint64_t idle);
    virtual void *replace(RemoteCacheBase& rcb, const void *key, const void* val, uint64_t life, uint64_t idle);
    virtual void *remove(RemoteCacheBase& rcb, const void* key);
    bool  containsKey(RemoteCacheBase& rcb, const void* key);
    virtual bool  replaceWithVersion(RemoteCacheBase& rcb, const void* k, const void* v, uint64_t version, uint64_t life, uint64_t idle);
    virtual bool  removeWithVersion(RemoteCacheBase& rcb, const void* k, uint64_t version);
    void *getWithMetadata(RemoteCacheBase& rcb, const void *key, MetadataValue* metadata);
    virtual void *getWithVersion(RemoteCacheBase& rcb, const void *key, VersionedValue* version);
    void  getBulk(RemoteCacheBase& rcb, std::map<void*, void*> &mbuf);
    void  getBulk(RemoteCacheBase& rcb, int size,  std::map<void*, void*> &mbuf);
    void  keySet(RemoteCacheBase& rcb, int scope, std::vector<void*> &result);
    void  stats(std::map<std::string,std::string> &stats);
    virtual void clear();
    uint64_t size();
    std::vector<char> execute(std::vector<char> cmdName, const std::map<std::vector<char>,std::vector<char>>& args);
    QueryResponse query(const QueryRequest & qr);
    operations::PingResult ping();
    CacheTopologyInfo getCacheTopologyInfo();
    void addClientListener(ClientListener&, const std::vector<std::vector<char> >, const std::vector<std::vector<char> >, const std::function<void()> &);
    void removeClientListener(ClientListener&);
    virtual void init(operations::OperationsFactory* operationsFactory);

    void withFlags(Flag flag);

    const char *getName() const;

private:
    RemoteCacheManagerImpl& remoteCacheManager;
    std::shared_ptr<operations::OperationsFactory> operationsFactory;
    std::string name;

    void applyDefaultExpirationFlags(uint64_t lifespan, uint64_t maxIdle);
    void assertRemoteCacheManagerIsStarted();
};

class KeyUnmarshallerFtor {
private:
    RemoteCacheBase &base;
public:
    KeyUnmarshallerFtor(RemoteCacheBase &b): base(b) {}
    void *operator()(const std::vector<char> &bytes) {
        return base.baseKeyUnmarshall(bytes);
    }
};

class ValueUnmarshallerFtor {
private:
    RemoteCacheBase &base;
public:
    ValueUnmarshallerFtor(RemoteCacheBase &b): base(b) {}
    void *operator()(const std::vector<char> &bytes) {
        return base.baseKeyUnmarshall(bytes);
    }
};

}} // namespace infinispan::hotrod

#endif  /* ISPN_HOTROD_REMOTECACHEIMPL_H */

