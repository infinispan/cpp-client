#ifndef ISPN_HOTROD_OPERATIONS_ABSTRACTKEYOPERATION_H
#define ISPN_HOTROD_OPERATIONS_ABSTRACTKEYOPERATION_H



#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/VersionedOperationResponse.h"
#include "infinispan/hotrod/Flag.h"
#include "hotrod/impl/protocol/HotRodConstants.h"

#include <set>
#include <sstream>

namespace infinispan {
namespace hotrod {
class IntWrapper;
namespace operations {

template<class T> class AbstractKeyOperation : public RetryOnFailureOperation<T>
{
  protected:
    const hrbytes& key;

    AbstractKeyOperation(
        const protocol::Codec& _codec,
        std::shared_ptr<transport::TransportFactory> _transportFactory,
        const hrbytes& _key, const hrbytes& _cacheName,
        IntWrapper& _topologyId, uint32_t  _flags) :
            RetryOnFailureOperation<T>(
                _codec, _transportFactory, _cacheName, _topologyId, _flags),
            key(_key)
        {}

    transport::Transport& getTransport(int retryCount) {
        if (retryCount == 0) {
            return this->transportFactory->getTransport(key, this->cacheName);
        } else {
            return this->transportFactory->getTransport(this->cacheName);
        }
    }

    uint8_t sendKeyOperation(
        const hrbytes& _key, transport::Transport& transport,
        uint8_t opCode, uint8_t /*opRespCode*/)
    {
        // 1) write [header][key length][key]
        hr_scoped_ptr<protocol::HeaderParams> params(
            &(this->writeHeader(transport, opCode)));
        transport.writeArray(_key);
        transport.flush();

        // 2) now read the header
        return this->readHeaderAndValidate(transport, *params);
    }

    hrbytes returnPossiblePrevValue(transport::Transport& transport, uint8_t status) {
    	return this->codec.returnPossiblePrevValue(transport, status, this->flags);
    }

    VersionedOperationResponse returnVersionedOperationResponse(
        transport::Transport& transport, protocol::HeaderParams& params)
    {
        uint8_t status =
            this->readHeaderAndValidate(transport, params);
        VersionedOperationResponse::RspCode code;
        if (status == protocol::HotRodConstants::NO_ERROR_STATUS) {
           	code = VersionedOperationResponse::SUCCESS;
        } else if (status == protocol::HotRodConstants::NOT_PUT_REMOVED_REPLACED_STATUS) {
            TRACE("Operation failed due to modification");
            code = VersionedOperationResponse::MODIFIED_KEY;
        } else if (status == protocol::HotRodConstants::KEY_DOES_NOT_EXIST_STATUS) {
            TRACE("Key does not exist!");
            code = VersionedOperationResponse::NO_SUCH_KEY;
        } else {
            TRACE("Error status %u", status);
            std::ostringstream message;
            message << "Unknown response status: " << std::hex << (int) status;
            // TODO: check exception type
            throw InvalidResponseException(message.str());
        }
        hrbytes prevValue = returnPossiblePrevValue(transport,status);
        return VersionedOperationResponse(prevValue, code);
    }


  private:
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_ABSTRACTKEYOPERATION_H

