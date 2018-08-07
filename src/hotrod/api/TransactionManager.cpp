/*
 * TransactionManager.cpp
 *
 *  Created on: Jul 4, 2018
 *      Author: rigazilla
 */

#include "infinispan/hotrod/TransactionManager.h"
#include "infinispan/hotrod/RemoteCacheBase.h"
#include "infinispan/hotrod/exceptions.h"
#include <thread>
#include <iostream>
#include <random>


namespace infinispan {
namespace hotrod {

const std::map<TransactionStatus, std::string> Transaction::mapStatusString = {
        { ACTIVE, "ACTIVE" }, {MARKED_ROLLBACK, "MARKED_ROLLBACK" }, { PREPARED, "PREPARED" },
        { COMMITTED, "COMMITTED" }, { ROLLEDBACK, "ROLLEDBACK" }, { UNKNOWN, "UNKNOWN" },
        { NO_TRANSACTION, "NO_TRANSACTION" }, { PREPARING, "PREPARING" }, { COMMITTING, "COMMITTING" },
        { ROLLING_BACK, "ROLLING_BACK" }
        };
const std::string UNKNOWN_STATUS("UNKNOWN_STATUS");
const std::string& Transaction::statusToString() {
    auto it = mapStatusString.find(this->status);
    return (it != mapStatusString.end()) ? it->second : UNKNOWN_STATUS;
}

XID::XID() {
    static std::default_random_engine e{};
    static std::uniform_int_distribution<int> d{0,255};
    std::vector<char> uuid(TransactionManagerLookup::lookup().getUuid());
    globalId.insert(globalId.end(), uuid.begin(), uuid.begin()+8);
    globalId.insert(globalId.end(), uuid.end()-8, uuid.end());
    for (auto i = 0; i < 16 ; i++) {
        globalId.push_back(d(e) & 0xff);
    }
    branchId.insert(branchId.end(), uuid.begin(), uuid.begin()+8);
    branchId.insert(branchId.end(), uuid.end()-8, uuid.end());
    for (auto i = 0; i < 16 ; i++) {
        branchId.push_back(d(e) & 0xff);
    }
}

bool XID::operator< (const XID& other) const {
    if (this->branchId < other.branchId) {
        return true;
    }
    if (this->branchId > other.branchId) {
        return false;
    }
    return this->globalId < other.globalId;
}

uint32_t XID::getFormatId() const {
    return 0x48525458;  // The hotrod format id
}

const std::vector<char>& XID::getGlobalTransactionId() const {
    return globalId;
}
const std::vector<char>& XID::getBranchQualifier() const {
    return branchId;
}

/* This function generate an random ID that seems a V4 UUID
 * it's not an implementation of UUID v4
 */
std::vector<char> TransactionManager::generateV4UUID()
{
    std::vector<char> tmp(16);
    static std::default_random_engine e{};
    static std::uniform_int_distribution<int> d{0,255};
    auto i=0;
    for (; i<16; i++)
    {
        tmp[i]=(unsigned char)d(e);
    }
    tmp[6] = (tmp[6] & 0x0F) | 0x40;
    tmp[8] = (tmp[8] & 0x3F) | 0x80;
    return tmp;
}

std::shared_ptr<Transaction> TransactionManager::getCurrentTransaction() {
    auto& ctPtr = currentTransactions[std::this_thread::get_id()];
    if (!ctPtr) {
        ctPtr.reset(new Transaction());
    }
    return ctPtr;
}

void TransactionManager::cleanUpCurrentTransaction() {
    auto& ctPtr = currentTransactions[std::this_thread::get_id()];
    if (ctPtr) {
        ctPtr->sa.clear();
        currentTransactions.erase(std::this_thread::get_id());
    }
}

void TransactionManager::begin() {
    infinispan::hotrod::Transaction& currentTransaction = *getCurrentTransaction();
    // TODO: check properly the tx status
    if (currentTransaction.canBegin()) {
        currentTransaction.status = ACTIVE;
    } else {
        this->throwExceptionOnIllegalState(currentTransaction.statusToString(), "begin()");
    }
}

void TransactionManager::commit() {
    infinispan::hotrod::Transaction& currentTransaction = *getCurrentTransaction();
    if (currentTransaction.canCommit()) {
        // Commit actually rolls back if transaction is marked as rolled back
        TransactionRemoteStatus exCode = TransactionRemoteStatus::XA_RBROLLBACK;
        if (currentTransaction.needsRollback()) {
            remoteRollback(currentTransaction);
            currentTransaction.status = ROLLEDBACK;
        } else {
            try {
                if (remotePrepareCommit(currentTransaction) == TransactionRemoteStatus::XA_OK) {
                    remoteCommit(currentTransaction);
                }
                currentTransaction.status = COMMITTED;
            } catch (const HotRodClientTxRemoteStateException& ex) {
                remoteRollback(currentTransaction);
                currentTransaction.status = ROLLEDBACK;
                exCode = (TransactionRemoteStatus) ex.getStatus();
            } catch (const HotRodClientException& ex) {
                remoteRollback(currentTransaction);
                currentTransaction.status = ROLLEDBACK;
            }
        }
        auto txStatus = currentTransaction.status;
        currentTransaction.sa.clear();
        cleanUpCurrentTransaction();
        if (txStatus == ROLLEDBACK) {
            throw HotRodClientRollbackException((unsigned int) exCode);
        }
    } else {
        this->throwExceptionOnIllegalState(currentTransaction.statusToString(), "commit()");
    }
}

void TransactionManager::rollback() {
    infinispan::hotrod::Transaction& currentTransaction = *getCurrentTransaction();
    if (currentTransaction.canRollback()) {
        remoteRollback(currentTransaction);
        currentTransaction.status = ROLLEDBACK;
        cleanUpCurrentTransaction();

    } else {
        this->throwExceptionOnIllegalState(currentTransaction.statusToString(), "rollback()");
    }
}

TransactionRemoteStatus TransactionManager::remotePrepareCommit(Transaction& t) {
    // For each caches listed as resource remotely call a PrepareCommit operation
    bool readOnly = true;
    for (auto& ctx : t.sa.registeredCache) {
        TransactionRemoteStatus ret = static_cast<TransactionRemoteStatus>(ctx.first->base_prepareCommit(t.xid, ctx.second));
        switch (ret) {
        case TransactionRemoteStatus::XA_OK:
            readOnly = false;
            t.sa.preparedCache[ctx.first] = &ctx.second;
            break;
        case TransactionRemoteStatus::XA_RDONLY:
            break;
        default:
            t.sa.preparedCache[ctx.first] = &ctx.second;
            throw HotRodClientTxRemoteStateException((unsigned int)ret);
        }
    }
    return readOnly ? TransactionRemoteStatus::XA_RDONLY : TransactionRemoteStatus::XA_OK;
}

void TransactionManager::remoteCommit(Transaction& t) {
    // For each caches listed as prepared remotely call a Commit operation
    bool hasCommit = false;
    bool hasRollback = false;
    bool alreadyForgot = false;
    bool unknown = false;
    unsigned int unknownErrorCode = 0;

    for (auto& ctx : t.sa.registeredCache) {
       TransactionRemoteStatus ret = static_cast<TransactionRemoteStatus>(ctx.first->base_commit(t.xid, ctx.second));
       switch (ret) {
          case TransactionRemoteStatus::XA_OK:       //no issues
          case TransactionRemoteStatus::XA_RDONLY:   //no issues
          case TransactionRemoteStatus::XA_HEURCOM: //heuristically committed
             hasCommit = true;
             break;
          case TransactionRemoteStatus::XAER_NOTA:
             //tx already forgotten by the server.
             alreadyForgot = true;
             break;
          case TransactionRemoteStatus::XA_HEURMIX: //completed. not sure how
          case TransactionRemoteStatus::XA_HEURHAZ: //heuristically committed and rolled back
             hasCommit = true;
             hasRollback = true;
             break;
          case TransactionRemoteStatus::XA_HEURRB: //heuristically rolled back
             hasRollback = true;
             break;
          default:
             //other error codes
             unknown = true;
             unknownErrorCode = (unsigned int)ret;
             break;
       }
    }

    if (!hasCommit && !hasRollback) {
       //nothing committed neither rolled back
       if (alreadyForgot) {
          //no tx
          throw HotRodClientTxRemoteStateException((unsigned int)TransactionRemoteStatus::XAER_NOTA);
       } else if (unknown) {
          //unknown. we throw what we received
          throw HotRodClientTxRemoteStateException(unknownErrorCode);
       }
    } else if (hasCommit && hasRollback) {
       //we have something committed and something rolled back
       throw HotRodClientTxRemoteStateException((unsigned int)TransactionRemoteStatus::XA_HEURMIX);
    } else if (hasRollback) {
       //rollbacks only
       throw HotRodClientTxRemoteStateException((unsigned int)TransactionRemoteStatus::XA_HEURRB);
    }
    // commits only.
}

void TransactionManager::remoteRollback(Transaction& t) {
   bool hasCommit = false;
   bool hasRollback = false;
   bool alreadyForgot = false;
   bool unknown = false;
   int unknownErrorCode = 0;

   for (auto& ctx : t.sa.preparedCache) {
      TransactionRemoteStatus ret = static_cast<TransactionRemoteStatus>(ctx.first->base_rollback(t.xid, *ctx.second));
      switch (ret) {
         case TransactionRemoteStatus::XA_OK:       //no issues
         case TransactionRemoteStatus::XA_RDONLY:   //no issues
         case TransactionRemoteStatus::XA_HEURRB:  //heuristically rolled back
            hasRollback = true;
            break;
         case TransactionRemoteStatus::XAER_NOTA:
            //tx already forgotten by the server.
            alreadyForgot = true;
            break;
         case TransactionRemoteStatus::XA_HEURMIX: //completed but not sure how
         case TransactionRemoteStatus::XA_HEURHAZ: //heuristically committed and rolled back
            hasCommit = true;
            hasRollback = true;
            break;
         case TransactionRemoteStatus::XA_HEURCOM: //heuristically committed
            hasCommit = true;
            break;
         default:
            //other error codes
            unknown = true;
            unknownErrorCode = (unsigned int)ret;
            break;
      }
   }

   if (!hasCommit && !hasRollback) {
      //nothing committed neither rolled back
      if (alreadyForgot) {
         //no tx
         throw HotRodClientTxRemoteStateException((unsigned int)TransactionRemoteStatus::XAER_NOTA);
      } else if (unknown) {
         //unknown. we throw what we received
         throw HotRodClientTxRemoteStateException(unknownErrorCode);
      }
   } else if (hasCommit && hasRollback) {
      //we have something committed and something rolled back
      throw HotRodClientTxRemoteStateException((unsigned int)TransactionRemoteStatus::XA_HEURMIX);
   } else if (hasCommit) {
      //we only got commits
      throw HotRodClientTxRemoteStateException((unsigned int)TransactionRemoteStatus::XA_HEURCOM);
   }
   //we only got rollbacks
}

void TransactionManager::throwExceptionOnIllegalState(const std::string& state, const std::string& action) {
    throw HotRodClientTxStateException(state, action);
}


SynchronizationAdapter& TransactionTable::getAdapter(Transaction& tx) {
    return tx.sa;
}

TransactionContext& SynchronizationAdapter::getTransactionContext(RemoteCacheBase* cache) {
    return registeredCache[cache];
}

void SynchronizationAdapter::clear() {
    preparedCache.clear();
    for (auto & it : registeredCache) {
        it.second.clear();
    }
    registeredCache.clear();
}


bool TransactionContext::hasValue(const std::vector<char>& key) {
    return (context.find(key) != context.end());
}

bool TransactionContext::isDeleted(const std::vector<char>& key) {
    std::map<const std::vector<char>, ContextEntry>::iterator found = context.find(key);
    return (found != context.end() && found->second.action == Action::DELETED);
}

const void* TransactionContext::getValue(const std::vector<char>& key) {
    ContextEntry& ce = context[key];
    return (ce.action == TransactionContext::DELETED) ? nullptr : ce.value;
}

TransactionContext::ContextEntry& TransactionContext::getEntry(std::vector<char>& key) {
    ContextEntry& ce = context[key];
    return ce;
}

void TransactionContext::setValue(std::vector<char>& key, const void* value, MetadataValue& meta, std::function<void (const void *)> deleter, std::function<void (const void *, std::vector<char>&)> valueMarshaller) {
    ContextEntry& ce = context[key];
    ce.action=TransactionContext::SET;
    ce.meta=meta;
    ce.meta.version=ce.origMeta.version+1;
    ce.changed = true;
    if (ce.value && ce.deleter) {
     ce.deleter(ce.value);
    }
    ce.deleter = deleter;
    ce.valueMarshaller = valueMarshaller;
    ce.value = value;
}

void TransactionContext::addValue(std::vector<char>& key, const void* value, MetadataValue& meta, std::function<void (const void *)> deleter, std::function<void (const void *, std::vector<char>&)> valueMarshaller, ControlBit cb) {
    context[key] = ContextEntry(TransactionContext::SET, value, meta, deleter, valueMarshaller);
    context[key].controlByte |= cb;
}

void TransactionContext::setValueAsDeleted(std::vector<char>& key) {
    ContextEntry& ce = context[key];
    ce.action=TransactionContext::DELETED;
    ce.meta=MetadataValue();
    ce.meta.version=ce.origMeta.version+1;
    ce.changed = true;
    if (ce.value && ce.deleter) {
        ce.deleter(ce.value);
    }
    ce.value = nullptr;
    ce.deleter = nullptr;
}

void TransactionContext::getBulk(int size, std::map<std::vector<char>, const void*> &mbuf) {
    for (auto const & it : context) {
        if (it.second.action!=Action::DELETED && it.second.value!=nullptr) {
            mbuf[it.first]=it.second.value;
        }
    }
}

void TransactionContext::removeEntry(const std::vector<char>& key) {
    ContextEntry& ce = context[key];
    if (ce.value && ce.deleter) {
        ce.deleter(ce.value);
    }
    context.erase(key);
}

void TransactionContext::clear() {
    for (auto & it  : context) {
        if (it.second.value && it.second.deleter) {
            it.second.deleter(it.second.value);
            it.second.value = nullptr;
            it.second.deleter = nullptr;
        }
    }
    context.clear();
}

int TransactionContext::size() {
    return context.size();
}

}
}

