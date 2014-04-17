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
        HR_SHARED_PTR<transport::TransportFactory> _transportFactory,
        const hrbytes& _key, const hrbytes& _cacheName,
        IntWrapper& _topologyId, uint32_t  _flags) :
            RetryOnFailureOperation<T>(
                _codec, _transportFactory, _cacheName, _topologyId, _flags),
            key(_key)
        {}

    transport::Transport& getTransport(int retryCount) {
        if (retryCount == 0) {
            return RetryOnFailureOperation<T>::transportFactory->getTransport(key);
        } else {
            return RetryOnFailureOperation<T>::transportFactory->getTransport();
        }
    }

    uint8_t sendKeyOperation(
        const hrbytes& _key, transport::Transport& transport,
        uint8_t opCode, uint8_t /*opRespCode*/)
    {
        // 1) write [header][key length][key]
        hr_scoped_ptr<protocol::HeaderParams> params(
            &(RetryOnFailureOperation<T>::writeHeader(transport, opCode)));
        transport.writeArray(_key);
        transport.flush();

        // 2) now read the header
        return RetryOnFailureOperation<T>::readHeaderAndValidate(transport, *params);
    }

    hrbytes returnPossiblePrevValue(transport::Transport& transport) {
        hrbytes result;
        if (hasForceReturn(RetryOnFailureOperation<T>::flags)) {
            TRACEBYTES("return value = ", result);
            result = transport.readArray();
        } else {
            TRACE("No return value");
        }
        return result;
    }

    VersionedOperationResponse returnVersionedOperationResponse(
        transport::Transport& transport, protocol::HeaderParams& params)
    {
        uint8_t status =
            RetryOnFailureOperation<T>::readHeaderAndValidate(transport, params);
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
        hrbytes prevValue = returnPossiblePrevValue(transport);
        return VersionedOperationResponse(prevValue, code);
    }


  private:
    bool hasForceReturn(uint32_t _flags) {
        if ((_flags & FORCE_RETURN_VALUE) != FORCE_RETURN_VALUE) {
            return false;
        }
        return true;
    }
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_ABSTRACTKEYOPERATION_H

