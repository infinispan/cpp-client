#include "infinispan/hotrod/RemoteCacheBase.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "hotrod/impl/RemoteCacheImpl.h"
#include "infinispan/hotrod/Query.h"
#include "../impl/RemoteCacheManagerImpl.h"
#include <iostream>
#include <memory>

namespace infinispan {
namespace hotrod {

#define IMPL ((RemoteCacheImpl *) impl.get())

void RemoteCacheBase::setRemoteCachePtr(void* ptr) {
    remoteCachePtr = ptr;
}
void RemoteCacheBase::setMarshallers(void* rc, MarshallHelperFn kf, MarshallHelperFn vf, UnmarshallHelperFn ukf, UnmarshallHelperFn uvf) {
    remoteCachePtr = rc;
    baseKeyMarshallFn = kf;
    baseValueMarshallFn = vf;
    baseKeyUnmarshallFn = ukf;
    baseValueUnmarshallFn = uvf;
    valueMarshallerFn = [this] (const void * v, std::vector<char> & mv) { this->baseValueMarshallFn(this, v, mv); };
}

void RemoteCacheBase::baseKeyMarshall(const void* k, std::vector<char> &buf) {
    baseKeyMarshallFn(remoteCachePtr, k, buf);
}

void RemoteCacheBase::baseValueMarshall(const void* v, std::vector<char> &buf) {
    baseValueMarshallFn(remoteCachePtr, v, buf);
}

void* RemoteCacheBase::baseKeyUnmarshall(const std::vector<char> &buf) {
    return baseKeyUnmarshallFn(remoteCachePtr, buf);
}

void* RemoteCacheBase::baseValueUnmarshall(const std::vector<char> &buf) {
    return baseValueUnmarshallFn(remoteCachePtr, buf);
}

const char *RemoteCacheBase::base_getName() {
    return IMPL->getName();
}

const std::string& RemoteCacheBase::base_getNameAsString() {
    return IMPL->getNameAsString();
}

// If a transaction is ongoing get the value for the key in a transactional way
void* RemoteCacheBase::transactional_base_get(Transaction& currentTransaction, const void* key) {
    if (!currentTransaction.acceptRead()) {
        throw HotRodClientTxStateException(currentTransaction.statusToString(), "get");
    }
    void* retVal = nullptr;
    const void* value = nullptr;
    SynchronizationAdapter& sa = transactionTable.getAdapter(currentTransaction);
    TransactionContext& tc = sa.getTransactionContext(this);
    std::vector<char> marshalledKey;
    baseKeyMarshall(key, marshalledKey);
    // If the transactional context hasn't the value get it from remote and add it to the context
    if (!tc.hasValue(marshalledKey)) {
        MetadataValue meta;
        value = IMPL->getWithMetadata(*this, key, &meta);
        tc.addValue(marshalledKey, value, meta, this->valueDestructor, this->valueMarshallerFn, (value ? TransactionContext::ControlBit::NONE : TransactionContext::ControlBit::NON_EXISTING));
    } else {
        value = tc.getValue(marshalledKey);
    }
    // Returning a copy as does the non tx get
    if (value != nullptr) {
        retVal = valueCopyConstructor(tc.getValue(marshalledKey));
    }
    return retVal;
}

// If a transaction is ongoing put the (key,value) pair into the tx context
void* RemoteCacheBase::transactional_base_put(Transaction& currentTransaction, const void* key, const void* val,
        int64_t life, int64_t idle, bool forceRV) {
    // If return previous value is true, get it
    if (!currentTransaction.acceptRW()) {
        throw HotRodClientTxStateException(currentTransaction.statusToString(), "put");
    }
    void* retVal = (forceRV) ? transactional_base_get(currentTransaction, key) : nullptr;
    SynchronizationAdapter& sa = transactionTable.getAdapter(currentTransaction);
    TransactionContext& tc = sa.getTransactionContext(this);
    std::vector<char> marshalledKey;
    baseKeyMarshall(key, marshalledKey);
    MetadataValue meta;
    meta.lifespan = life;
    meta.maxIdle = idle;
    // TX context need to handle it's own copy of the value
    void* valueCopy = valueCopyConstructor(val);
    if (tc.hasValue(marshalledKey)) {
    tc.setValue(marshalledKey, valueCopy, meta, valueDestructor, valueMarshallerFn);
    } else {
        tc.addValue(marshalledKey, valueCopy, meta, valueDestructor, valueMarshallerFn, TransactionContext::ControlBit::NOT_READ);
    }
    return retVal;
}

void *RemoteCacheBase::base_get(const void *key, std::shared_ptr<Transaction> currentTxPtr) {
    if (transactional) {
        Transaction& currentTransaction = currentTxPtr ? *currentTxPtr : *transactionManager.getCurrentTransaction();
        if (currentTransaction.getStatus() != NO_TRANSACTION) {
            return transactional_base_get(currentTransaction, key);
        }
    }
    return IMPL->get(*this, key);
}

std::map<std::vector<char>, std::vector<char>> RemoteCacheBase::base_getAll(const std::set<std::vector<char>>& keySet) {
    return IMPL->getAll(keySet);
}

void *RemoteCacheBase::base_put(const void *key, const void *val, int64_t life, int64_t idle,
        std::shared_ptr<Transaction> currentTxPtr) {
    if (transactional) {
        Transaction& currentTransaction = currentTxPtr ? *currentTxPtr : *transactionManager.getCurrentTransaction();
        if (currentTransaction.getStatus() != NO_TRANSACTION) {
            return transactional_base_put(currentTransaction, key, val, life, idle, forceReturnValue);
        }
    }
    return IMPL->put(*this, key, val, life, idle);
}

void RemoteCacheBase::base_putAll(const std::map<const void*, const void*>& map, int64_t life, int64_t idle,
        std::shared_ptr<Transaction> currentTxPtr) {
    for (auto const& it : map)
    {
        base_put(it.first, it.second, life, idle, currentTxPtr);
    }

}

void *RemoteCacheBase::base_putIfAbsent(const void *key, const void *val, int64_t life, int64_t idle,
        std::shared_ptr<Transaction> currentTxPtr) {
    if (transactional) {
        Transaction& currentTransaction = currentTxPtr ? *currentTxPtr : *transactionManager.getCurrentTransaction();
        if (currentTransaction.getStatus() != NO_TRANSACTION) {
            void* retVal = transactional_base_get(currentTransaction, key);
            if (retVal == nullptr) {
                transactional_base_put(currentTransaction, key, val, life, idle, false);
            }
            return nullptr;
        }
    }
    return IMPL->putIfAbsent(*this, key, val, life, idle);
}

void *RemoteCacheBase::base_replace(const void *key, const void *val, int64_t life, int64_t idle,
        std::shared_ptr<Transaction> currentTxPtr) {
    if (transactional) {
        Transaction& currentTransaction = currentTxPtr ? *currentTxPtr : *transactionManager.getCurrentTransaction();
        if (currentTransaction.getStatus() != NO_TRANSACTION) {
            if (!currentTransaction.acceptRW()) {
                throw HotRodClientTxStateException(currentTransaction.statusToString(), "replace");
            }
            void* retVal = transactional_base_get(currentTransaction, key);
            if (retVal != nullptr) {
                SynchronizationAdapter& sa = transactionTable.getAdapter(currentTransaction);
                TransactionContext& tc = sa.getTransactionContext(this);
                std::vector<char> marshalledKey;
                baseKeyMarshall(key, marshalledKey);
                MetadataValue meta;
                meta.lifespan = life;
                meta.maxIdle = idle;
                tc.setValue(marshalledKey, val, meta, valueDestructor, valueMarshallerFn);
                if (forceReturnValue) {
                    // the old value is not reference into the tx map
                    // avoid a copy and destroy operation and return it instead
                    return (void *) retVal;
                } else {
                    // Destroy old object
                    valueDestructor(retVal);
                }
            }
            return nullptr;
        }
    }
    return IMPL->replace(*this, key, val, life, idle);
}

void *RemoteCacheBase::base_remove(const void *key, std::shared_ptr<Transaction> currentTxPtr) {
    if (transactional) {
        Transaction& currentTransaction = currentTxPtr ? *currentTxPtr : *transactionManager.getCurrentTransaction();
        if (currentTransaction.getStatus() != NO_TRANSACTION) {
            if (!currentTransaction.acceptRW()) {
                throw HotRodClientTxStateException(currentTransaction.statusToString(), "remove");
            }
            void* retVal = (forceReturnValue) ? transactional_base_get(currentTransaction, key) : nullptr;
            std::vector<char> marshalledKey;
            baseKeyMarshall(key, marshalledKey);
            SynchronizationAdapter& sa = transactionTable.getAdapter(currentTransaction);
            TransactionContext& tc = sa.getTransactionContext(this);
            tc.setValueAsDeleted(marshalledKey);
            return retVal;
        }
    }
    return IMPL->remove(*this, key);
}

bool RemoteCacheBase::base_containsKey(const void *key, std::shared_ptr<Transaction> currentTxPtr) {
    if (transactional) {
        Transaction& currentTransaction = currentTxPtr ? *currentTxPtr : *transactionManager.getCurrentTransaction();
        if (currentTransaction.getStatus() != NO_TRANSACTION) {
            if (!currentTransaction.acceptRead()) {
                throw HotRodClientTxStateException(currentTransaction.statusToString(), "containsKey");
            }
            std::vector<char> marshalledKey;
            baseKeyMarshall(key, marshalledKey);
            SynchronizationAdapter& sa = transactionTable.getAdapter(currentTransaction);
            TransactionContext& tc = sa.getTransactionContext(this);
            if (tc.getValue(marshalledKey) != nullptr) {
                return true;
            }
        }
    }
    return IMPL->containsKey(*this, key);
}

bool RemoteCacheBase::base_replaceWithVersion(const void *key, const void *value, int64_t version, int64_t life,
        int64_t idle, std::shared_ptr<Transaction> currentTxPtr)
        {
    if (transactional) {
        Transaction& currentTransaction = currentTxPtr ? *currentTxPtr : *transactionManager.getCurrentTransaction();
        if (currentTransaction.getStatus() != NO_TRANSACTION) {
            if (!currentTransaction.acceptRW()) {
                throw HotRodClientTxStateException(currentTransaction.statusToString(), "replaceWithVersion");
            }
            void* retVal = transactional_base_get(currentTransaction, key);
            if (retVal != nullptr) {
                SynchronizationAdapter& sa = transactionTable.getAdapter(currentTransaction);
                TransactionContext& tc = sa.getTransactionContext(this);
                std::vector<char> marshalledKey;
                baseKeyMarshall(key, marshalledKey);
                TransactionContext::ContextEntry& entry = tc.getEntry(marshalledKey);
                if (entry.meta.version == version) {
                    MetadataValue newMeta;
                    newMeta.lifespan = life;
                    newMeta.maxIdle = idle;
                    newMeta.version = entry.meta.version + 1;
                    tc.setValue(marshalledKey, value, newMeta, valueDestructor, valueMarshallerFn);
                    // Destroy old object
                    valueDestructor(retVal);
                    return true;
                }
            }
            return false;
        }
    }
    return IMPL->replaceWithVersion(*this, key, value, version, life, idle);
}

bool RemoteCacheBase::base_removeWithVersion(const void *key, int64_t version, std::shared_ptr<Transaction> currentTxPtr)
        {
    if (transactional) {
        Transaction& currentTransaction = currentTxPtr ? *currentTxPtr : *transactionManager.getCurrentTransaction();
        if (currentTransaction.getStatus() != NO_TRANSACTION) {
            if (!currentTransaction.acceptRW()) {
                throw HotRodClientTxStateException(currentTransaction.statusToString(), "removeWithVersion");
            }
            void* retVal = transactional_base_get(currentTransaction, key);
            if (retVal != nullptr) {
                SynchronizationAdapter& sa = transactionTable.getAdapter(currentTransaction);
                TransactionContext& tc = sa.getTransactionContext(this);
                std::vector<char> marshalledKey;
                baseKeyMarshall(key, marshalledKey);
                TransactionContext::ContextEntry& entry = tc.getEntry(marshalledKey);
                if (entry.meta.version == version) {
                    MetadataValue newMeta;
                    newMeta.version = entry.meta.version + 1;
                    tc.setValueAsDeleted(marshalledKey);
                    // Destroy old object
                    valueDestructor(retVal);
                    return true;
                }
            }
            return false;
        }
    }
    return IMPL->removeWithVersion(*this, key, version);
}

void *RemoteCacheBase::base_getWithVersion(const void *key, VersionedValue* version, std::shared_ptr<Transaction> currentTxPtr)
        {
    if (transactional) {
        Transaction& currentTransaction = currentTxPtr ? *currentTxPtr : *transactionManager.getCurrentTransaction();
        if (currentTransaction.getStatus() != NO_TRANSACTION) {
            return transactional_base_get(currentTransaction, key);
        }
    }
    return IMPL->getWithVersion(*this, key, version);
}

void *RemoteCacheBase::base_getWithMetadata(const void *key, MetadataValue* metadata, std::shared_ptr<Transaction> currentTxPtr)
        {
    if (transactional) {
        Transaction& currentTransaction = currentTxPtr ? *currentTxPtr : *transactionManager.getCurrentTransaction();
        if (currentTransaction.getStatus() != NO_TRANSACTION) {
            return transactional_base_get(currentTransaction, key);
        }
    }
    return IMPL->getWithMetadata(*this, key, metadata);
}

void RemoteCacheBase::base_getBulk(int size, std::map<void*, void*> &mbuf, std::shared_ptr<Transaction> currentTxPtr)
        {
    if (transactional) {
        Transaction& currentTransaction = currentTxPtr ? *currentTxPtr : *transactionManager.getCurrentTransaction();
        if (currentTransaction.getStatus() != NO_TRANSACTION) {
            // If there's a transaction context then get the first n values from there first
            // go remote for the remaining
            if (!currentTransaction.acceptRead()) {
                throw HotRodClientTxStateException(currentTransaction.statusToString(), "getBulk");
            }
            SynchronizationAdapter& sa = transactionTable.getAdapter(currentTransaction);
            TransactionContext& tc = sa.getTransactionContext(this);
            std::map<std::vector<char>, const void *> ctxEntries;
            tc.getBulk(size, ctxEntries);
            for (auto const & item : ctxEntries) {
                mbuf[baseKeyUnmarshall(item.first)] = valueCopyConstructor(item.second);
            }
            std::map<void *, void *> remoteEntries;
            IMPL->getBulk(*this, size - ctxEntries.size(), remoteEntries);
            mbuf.insert(remoteEntries.begin(), remoteEntries.end());
        }
    }
    IMPL->getBulk(*this, size, mbuf);
}

void RemoteCacheBase::base_keySet(int scope, std::vector<void*> &result)
{
    IMPL->keySet(*this, scope, result);
}

void RemoteCacheBase::base_stats(std::map<std::string,std::string> &stats)
{
    IMPL->stats(stats);
}

void RemoteCacheBase::base_clear()
{
    IMPL->clear();
}

uint64_t RemoteCacheBase::base_size()
{
    return IMPL->size();
}

void RemoteCacheBase::base_ping() {
    IMPL->ping();
}

void RemoteCacheBase::base_withFlags(Flag flags) {
    IMPL->withFlags(flags);
}
std::vector<unsigned char> RemoteCacheBase::base_execute(const std::string &cmdName, const std::map<std::string,std::string>& args){
	std::map<std::vector<char>,std::vector<char>> m;
    std::vector<char> cmdNameBuf;
//    std::vector<char> argNameBuf, argValueBuf;
	for (std::map<std::string,std::string>::const_iterator it=args.begin(); it!=args.end(); ++it)
	{
        std::vector<char> argNameBytes(it->first.begin(),it->first.end());
        std::vector<char> argValueBytes(it->second.begin(),it->second.end());
        m.insert(std::pair<std::vector<char>,std::vector<char>>(argNameBytes, argValueBytes));
	}
    const std::vector<char> cmdNameBytes((cmdName.data()),(cmdName.data())+cmdName.size());
	std::vector<char> resBytes= IMPL->execute(cmdNameBytes, m);

	std::vector<unsigned char> ures(reinterpret_cast<unsigned char*>(resBytes.data()),reinterpret_cast<unsigned char*>(resBytes.data()+resBytes.size()));
	return ures;
}

std::vector<unsigned char> RemoteCacheBase::base_execute(const std::string &cmdName, const std::map<std::vector<char> ,std::vector<char> >& args){
        const std::vector<char> cmdNameBytes((cmdName.data()),(cmdName.data())+cmdName.size());
	std::vector<char> resBytes= IMPL->execute(cmdNameBytes, args);

	std::vector<unsigned char> ures(reinterpret_cast<unsigned char*>(resBytes.data()),reinterpret_cast<unsigned char*>(resBytes.data()+resBytes.size()));
	return ures;
}

std::vector<char> RemoteCacheBase::base_execute(const std::vector<char> &cmdName, const std::map<std::vector<char> ,std::vector<char> >& args){
    return IMPL->execute(cmdName, args);
}

QueryResponse RemoteCacheBase::base_query(const QueryRequest &qr)
{
	return IMPL->query(qr);
}

std::vector<unsigned char> RemoteCacheBase::base_query_char(std::vector<unsigned char> qr, size_t size)
{
	QueryRequest req;
	req.ParseFromArray(qr.data(),(int)size);
	if (!req.has_local()) {
		req.set_local(false);
	}
	QueryResponse resp= IMPL->query(req);

	int respSize = resp.ByteSizeLong();
	std::vector<unsigned char> respToChar(respSize);
        resp.SerializeToArray(respToChar.data(),respSize);
	return respToChar;
}



CacheTopologyInfo RemoteCacheBase::base_getCacheTopologyInfo() {
	TRACE("Calling RemoteCache for getCacheTopologyInfo");
	CacheTopologyInfo c=IMPL->getCacheTopologyInfo();
	TRACE("segmentPerServer: %d",c.getSegmentPerServer().size());
	return c;
}

void RemoteCacheBase::base_addClientListener(ClientListener& clientListener, const std::vector<std::vector<char> > filterFactoryParam, const std::vector<std::vector<char> > converterFactoryParams, const std::function<void()> &recoveryCallback)
{
	IMPL->addClientListener(clientListener, filterFactoryParam, converterFactoryParams, recoveryCallback);
}

void RemoteCacheBase::base_removeClientListener(ClientListener& clientListener)
{
	IMPL->removeClientListener(clientListener);
}

void RemoteCacheBase::putScript(const std::vector<char>& name, const std::vector<char>& script) {
    std::shared_ptr<RemoteCacheImpl> rcImpl = this->impl->remoteCacheManager.createRemoteCache("___script_cache", false,
            NearCacheConfiguration());
    rcImpl->setDataFormat(this->impl->dataFormat);
    rcImpl->putraw(name, script, 0, 0);
}

uint32_t RemoteCacheBase::base_prepareCommit(XID xid, TransactionContext& tctx) {
    return IMPL->prepareCommit(xid, tctx);
}

uint32_t RemoteCacheBase::base_commit(XID xid, TransactionContext& tctx) {
    return IMPL->commit(xid, tctx);
}

uint32_t RemoteCacheBase::base_rollback(XID xid, TransactionContext& tctx) {
    return IMPL->rollback(xid, tctx);
}

void RemoteCacheBase::cloneImplWithDataFormat(EntryMediaTypes *df) {
    impl.reset(new RemoteCacheImpl(*impl));
    impl->setDataFormat(df); }

}
} /* namespace */

