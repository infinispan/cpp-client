/*
 * TransactionManager.h
 *
 *  Created on: Jul 3, 2018
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_TRANSACTIONMANAGER_H_
#define INCLUDE_INFINISPAN_HOTROD_TRANSACTIONMANAGER_H_

#include "infinispan/hotrod/ImportExport.h"
#include <vector>
#include <memory>
#include <thread>
#include <map>
namespace infinispan {
namespace hotrod {

class Transaction;
enum class TransactionRemoteStatus : unsigned int;
/**
 * The transaction manager. Default behavior is a single transaction manager for application
 */
class TransactionManager {
public:
    HR_EXTERN TransactionManager() : UUID(generateV4UUID()) {}
    /***
     * Start a transaction on this thread. All the caches of all RemoteCacheManager
     * will be involved
     */
    HR_EXTERN void begin();
    /***
     * Commit this transaction on the remote server
     */
    HR_EXTERN void commit();
    /***
     * Rollback this transaction on the remote server
     */
    HR_EXTERN void rollback();
    /***
     * Suspend the current transaction. Not Implemented
     */
    // HR_EXTERN void suspend();
    /***
     * Resume the suspended current transaction. Not Implemented
     */
    // HR_EXTERN void resume();
    /***
     * Get the UUID of this transaction manager
     */
    const std::vector<char>& getUuid() const { return UUID; }
    HR_EXTERN std::shared_ptr<Transaction> getCurrentTransaction();
private:
    std::map<std::thread::id, std::shared_ptr<Transaction> > currentTransactions;
    std::vector<char> UUID;

    std::vector<char> generateV4UUID();
    void throwExceptionOnIllegalState(const std::string& state, const std::string& action);
    HR_EXTERN TransactionRemoteStatus remotePrepareCommit(Transaction& t);
    HR_EXTERN void cleanUpCurrentTransaction();
    HR_EXTERN void remoteCommit(Transaction& t);
    HR_EXTERN void remoteRollback(Transaction& t);
    HR_EXTERN void setRollbackOnly();
};

/**
 * The default transaction manager lookup class
 */
class TransactionManagerLookup {
public:
    /***
     * This provides the default instance of the transaction manager.
     * Used only internally for now
     */
    static TransactionManager& lookup() {
        static TransactionManager the_transaction_manager;
        return the_transaction_manager;
    }
};

}
}

#endif /* INCLUDE_INFINISPAN_HOTROD_TRANSACTIONMANAGER_H_ */
