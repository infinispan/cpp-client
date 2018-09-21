/*
 * TransactionOperations.cpp
 *
 *  Created on: Jul 20, 2018
 *      Author: rigazilla
 */

#include <hotrod/impl/operations/TransactionOperations.h>
#include <infinispan/hotrod/TimeUnit.h>

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

static void writeXID(transport::Transport& transport, XID& xid);

PrepareCommitOperation::PrepareCommitOperation(const Codec &codec, std::shared_ptr<TransportFactory> transportFactory,
        std::vector<char> cacheName, Topology& topologyId, int flags, XID xid, TransactionContext& tctx, EntryMediaTypes* df) :
        RetryOnFailureOperation<uint32_t>(codec, transportFactory, cacheName, topologyId, flags, df), xid(xid), tctx(tctx) {

}

uint32_t PrepareCommitOperation::executeOperation(transport::Transport& transport) {
    std::unique_ptr<HeaderParams> params(this->writeHeader(transport, PREPARE_REQUEST));
    writeXID(transport, this->xid);
    transport.writeByte(0); // One phase commit on XA transaction not implemented here
    transport.writeVInt(tctx.size());
    for ( auto & it : tctx.context) {
        transport.writeArray(it.first);
        unsigned char controlByte = it.second.controlByte;
        transport.writeByte(controlByte);
        if (! (controlByte & (TransactionContext::ControlBit::NON_EXISTING | TransactionContext::ControlBit::NOT_READ)) ) {
            transport.writeLong(it.second.origMeta.version);
        }
        if (! (controlByte & TransactionContext::ControlBit::REMOVE_OP) ) {
            transport.writeByte(0x77);  // both second for lifespan and maxidle
            if (it.second.meta.lifespan > 0) {
                transport.writeVLong(it.second.meta.lifespan);
            }
            if (it.second.meta.maxIdle > 0) {
                transport.writeVLong(it.second.meta.maxIdle);
            }
            std::vector<char> marshalledVal;
            it.second.valueMarshaller(it.second.value, marshalledVal);
            transport.writeArray(marshalledVal);
        }
    }
    transport.flush();
    uint8_t status = this->readHeaderAndValidate(transport, *params);
    // TODO: check for error
    uint32_t xa_retcode = transport.read4ByteInt();
    return xa_retcode;
}

CommitOperation::CommitOperation(const Codec &codec, std::shared_ptr<TransportFactory> transportFactory,
        std::vector<char> cacheName, Topology& topologyId, int flags, XID xid, TransactionContext& tctx, EntryMediaTypes* df) :
        RetryOnFailureOperation<uint32_t>(codec, transportFactory, cacheName, topologyId, flags, df), xid(xid), tctx(tctx) {

}

uint32_t CommitOperation::executeOperation(transport::Transport& transport) {
    std::unique_ptr<HeaderParams> params(this->writeHeader(transport, COMMIT_REQUEST));
    writeXID(transport, this->xid);
    transport.flush();
    uint8_t status = this->readHeaderAndValidate(transport, *params);
    // TODO: check for error
    uint32_t xa_retcode = transport.read4ByteInt();
    return xa_retcode;
}

RollbackOperation::RollbackOperation(const Codec &codec, std::shared_ptr<TransportFactory> transportFactory,
        std::vector<char> cacheName, Topology& topologyId, int flags, XID xid, TransactionContext& tctx, EntryMediaTypes* df) :
        RetryOnFailureOperation<uint32_t>(codec, transportFactory, cacheName, topologyId, flags, df), xid(xid), tctx(tctx) {

}

uint32_t RollbackOperation::executeOperation(transport::Transport& transport) {
    std::unique_ptr<HeaderParams> params(this->writeHeader(transport, ROLLBACK_REQUEST));
    writeXID(transport, this->xid);
    transport.flush();
    uint8_t status = this->readHeaderAndValidate(transport, *params);
    // TODO: check for error
    uint32_t xa_retcode = transport.read4ByteInt();
    return 0;
}

static void writeXID(transport::Transport& transport, XID& xid) {
    transport.writeVInt(xid.getFormatId()<<1 ^ xid.getFormatId()>>31);
    transport.writeArray(xid.getGlobalTransactionId());
    transport.writeArray(xid.getBranchQualifier());
}

}
}
}
