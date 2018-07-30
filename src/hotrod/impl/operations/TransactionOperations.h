/*
 * TransactionOperations.h
 *
 *  Created on: Jul 20, 2018
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_OPERATIONS_TRANSACTIONOPERATIONS_H_
#define SRC_HOTROD_IMPL_OPERATIONS_TRANSACTIONOPERATIONS_H_

#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/VersionedOperationResponse.h"
#include "infinispan/hotrod/Flag.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "infinispan/hotrod/TransactionManager.h"
#include "infinispan/hotrod/Transactions.h"

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

class PrepareCommitOperation: public RetryOnFailureOperation<uint32_t> {
public:
    PrepareCommitOperation(const Codec &codec, std::shared_ptr<TransportFactory> transportFactory,
            std::vector<char> cacheName, Topology& topologyId, int flags, XID xid, TransactionContext& tctx);
    uint32_t executeOperation(transport::Transport& transport);

private:
    XID xid;
    TransactionContext& tctx;
};

class CommitOperation: public RetryOnFailureOperation<uint32_t> {
public:
    CommitOperation(const Codec &codec, std::shared_ptr<TransportFactory> transportFactory,
            std::vector<char> cacheName, Topology& topologyId, int flags, XID xid, TransactionContext& tctx);
    uint32_t executeOperation(transport::Transport& transport);

private:
    XID xid;
    TransactionContext& tctx;
};

class RollbackOperation: public RetryOnFailureOperation<uint32_t> {
public:
    RollbackOperation(const Codec &codec, std::shared_ptr<TransportFactory> transportFactory,
            std::vector<char> cacheName, Topology& topologyId, int flags, XID xid, TransactionContext& tctx);
    uint32_t executeOperation(transport::Transport& transport);

private:
    XID xid;
    TransactionContext& tctx;
};

}
}
}

#endif /* SRC_HOTROD_IMPL_OPERATIONS_TRANSACTIONOPERATIONS_H_ */
