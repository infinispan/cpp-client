#ifndef ISPN_HOTROD_OPERATIONS_ABSTRACTKEYOPERATION_H
#define ISPN_HOTROD_OPERATIONS_ABSTRACTKEYOPERATION_H

#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "infinispan/hotrod/Flag.h"

#include <set>

namespace infinispan {
namespace hotrod {
namespace operations {

template<class T> class AbstractKeyOperation : public RetryOnFailureOperation<T>
{
    protected:
        AbstractKeyOperation(
            const infinispan::hotrod::protocol::Codec&       codec_,
            infinispan::hotrod::transport::TransportFactory& transportFactory_,
            const hrbytes&                                   key_,
            const hrbytes&                                   cacheName_,
            uint32_t                                         topologyId_,
            const std::set<Flag>&                                   flags_)
            : RetryOnFailureOperation<T>(codec_, transportFactory_,
                                      cacheName_,
                                      topologyId_, flags_), key(key_)
        {}

        infinispan::hotrod::transport::Transport* getTransport(int retryCount)
        {
            if (retryCount == 0) {
                return RetryOnFailureOperation<T>::transportFactory.getTransport(key);
            } else {
                return RetryOnFailureOperation<T>::transportFactory.getTransport();
            }
        }

        uint8_t sendKeyOperation(
            const hrbytes&                                      key_,
            infinispan::hotrod::transport::Transport&     transport,
            uint8_t                                       opCode,
            uint8_t                                       /*opRespCode*/)
        {
           // 1) write [header][key length][key]
           hr_scoped_ptr<infinispan::hotrod::protocol::HeaderParams> params(&(RetryOnFailureOperation<T>::writeHeader(transport, opCode)));
           transport.writeArray(key_);
           transport.flush();

           // 2) now read the header
           return RetryOnFailureOperation<T>::readHeaderAndValidate(transport, *params);
        }

        hrbytes returnPossiblePrevValue(infinispan::hotrod::transport::Transport& transport) {
            hrbytes result;
            if (hasForceReturn(HotRodOperation<T>::flags)) {
               result = transport.readArray();
            }
            result.setSmart(0,0);
            return result;
         }


           const hrbytes& key;

    private:
        bool hasForceReturn(const std::set<Flag>& flags_) {
            if (flags_.find(FORCE_RETURN_VALUE) != flags_.end()) {
                return true;
            }
            return false;
        }

};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_ABSTRACTKEYOPERATION_H

