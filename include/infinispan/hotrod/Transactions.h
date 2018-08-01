/*
 * Transactions.h
 *
 *  Created on: Jul 27, 2018
 *      Author: rigazilla
 */

#ifndef INFINISPAN_HOTROD_CPP_SRC_HOTROD_API_TRANSACTIONS_H_
#define INFINISPAN_HOTROD_CPP_SRC_HOTROD_API_TRANSACTIONS_H_

#include "infinispan/hotrod/MetadataValue.h"
#include "infinispan/hotrod/ImportExport.h"
#include <map>
#include <thread>
#include <vector>
#include <functional>

namespace infinispan {
namespace hotrod {

 class XID {
public:
    HR_EXTERN XID();
    HR_EXTERN bool operator< (const XID& other) const;
    HR_EXTERN uint32_t getFormatId() const;
    HR_EXTERN const std::vector<char>& getGlobalTransactionId() const;
    HR_EXTERN const std::vector<char>& getBranchQualifier() const;
private:
    std::vector<char> globalId;
    std::vector<char> branchId;
};
/**
 * The transaction class
 */
class TransactionManager;
class SynchronizationAdapter;
class TransactionTable;
class RemoteCacheBase;

namespace operations {
class PrepareCommitOperation;
}

// Replicating the status of a Java transaction
enum TransactionStatus {
    ACTIVE = 0,
    MARKED_ROLLBACK = 1,
    PREPARED = 2,
    COMMITTED = 3,
    ROLLEDBACK = 4,
    UNKNOWN = 5,
    NO_TRANSACTION = 6,
    PREPARING = 7,
    COMMITTING = 8,
    ROLLING_BACK = 9
};
// Decoding the remote tx operation return code
enum class TransactionRemoteStatus : unsigned int {
    XA_OK = 0x00,           // all ok
    XA_RDONLY = 0x03,       // branch is read only but that seems ok
    XA_HEURCOM = 0x07,      // resource committed after heuristic decision
    XAER_NOTA = 0xFFFFFFFC, // the XID is not valid
    XA_HEURMIX = 0x05,      // resource partially committed and rolledback after heuristic decision
    XA_HEURHAZ = 0x08,      // tx operation on resource completed somehow heuristically
    XA_HEURRB = 0x06        // resource rolled back after heuristic decision
};

/**
 * A transaction context for the RemoteCacheBase class
 */

class TransactionContext {
public:
    enum Action { SET, DELETED };
    enum ControlBit { NONE = 0x00, NOT_READ = 0x01, NON_EXISTING = 0x02, REMOVE_OP = 0x04 };
    struct ContextEntry {
        Action action;
        const void* value;
        MetadataValue origMeta;
        MetadataValue meta;
        bool changed;
        std::function<void (const void *)> deleter;
        std::function<void (const void *, std::vector<char>&)> valueMarshaller;
        unsigned char controlByte = NONE;
        ContextEntry() : action(SET), value(nullptr), changed(false) {}
        ContextEntry(Action action, const void* value, std::function<void (const void *)> deleter, std::function<void (const void *, std::vector<char>&)> valueMarshaller) : action(action), value(value), changed(false), deleter(deleter), valueMarshaller(valueMarshaller) {}
        ContextEntry(Action action, const void* value, MetadataValue& meta, std::function<void (const void *)> deleter, std::function<void (const void *, std::vector<char>&)> valueMarshaller) : action(action), value(value), origMeta(meta), meta(meta), changed(false), deleter(deleter), valueMarshaller(valueMarshaller) {}
        ~ContextEntry() {}
    };
    TransactionRemoteStatus getStatus() const
    {
        return status;
    }
    void setPrepared(TransactionRemoteStatus status)
            {
        this->status = status;
    }
    bool hasValue(const std::vector<char>& key);
    bool isDeleted(const std::vector<char>& key);
    const void* getValue(const std::vector<char>& key);
    ContextEntry& getEntry(std::vector<char>& key);
    void addValue(std::vector<char>& key, const void* value, MetadataValue& meta, std::function<void (const void *)> deleter, std::function<void (const void *, std::vector<char>&)> valueMarshaller, ControlBit cb = NONE);
    void setValue(std::vector<char>& key, const void* value, MetadataValue& meta, std::function<void (const void *)> deleter, std::function<void (const void *, std::vector<char>&)> valueMarshaller);
    void setValueAsDeleted(std::vector<char>& key);
    void removeEntry(const std::vector<char>& key);
    void clear();
    void getBulk(int size, std::map<std::vector<char>, const void*> &mbuf);
    int size();
private:
    std::map<const std::vector<char>, ContextEntry> context;
    friend TransactionManager;
    friend operations::PrepareCommitOperation;
    TransactionRemoteStatus status = TransactionRemoteStatus::XA_OK;
};

/**
 * The synchronization adapter between transaction and transaction manager
 */
class SynchronizationAdapter {
public:
    TransactionContext& getTransactionContext(RemoteCacheBase* cache);
    void clear();
private:
    std::map<RemoteCacheBase*, TransactionContext> registeredCache;
    std::map<RemoteCacheBase*, TransactionContext*> preparedCache;
    friend TransactionManager;
};

class Transaction {
public:
    // Construct with default status
    Transaction() :
            status(NO_TRANSACTION) {
    }
    TransactionStatus getStatus() const {
        return status;
    }
    bool canBegin() const {
        return status == NO_TRANSACTION || status == COMMITTED;
    }
    bool canCommit() const {
        return status == ACTIVE;
    }
    bool canRollback() const {
        return status == ACTIVE;
    }
    bool needsRollback() const {
        return status == MARKED_ROLLBACK;
    }
    bool acceptRW() const {
        return status == ACTIVE || status == MARKED_ROLLBACK;
    }
    bool acceptRead() const {
        return status != COMMITTED && status != ROLLEDBACK;
    }
    bool operator< (const Transaction& other) const { return this->xid < other.xid; }
    const std::string& statusToString();
private:
    TransactionStatus status;
    XID xid;
    SynchronizationAdapter sa;
    static const std::map<TransactionStatus, std::string> mapStatusString;
friend TransactionManager;
friend TransactionTable;
};

struct cmpByStringLength {
    bool operator()(const std::vector<char>& a, const std::vector<char>& b) const {
        return a.size() < b.size();
    }
};

class TransactionTable {
public:
    SynchronizationAdapter& getAdapter(Transaction& tx);
};

}}




#endif /* INFINISPAN_HOTROD_CPP_SRC_HOTROD_API_TRANSACTIONS_H_ */
