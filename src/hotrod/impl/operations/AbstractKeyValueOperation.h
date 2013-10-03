#ifndef ISPN_HOTROD_OPERATIONS_ABSTRACTKEYVALUEOPERATION_H
#define ISPN_HOTROD_OPERATIONS_ABSTRACTKEYVALUEOPERATION_H



#include "hotrod/impl/operations/AbstractKeyOperation.h"

#include <set>

namespace infinispan {
namespace hotrod {
namespace operations {

template<class T> class AbstractKeyValueOperation : public AbstractKeyOperation<T>
{
    protected:
	    AbstractKeyValueOperation(
            const protocol::Codec&       codec_,
            HR_SHARED_PTR<transport::TransportFactory> transportFactory_,
            const hrbytes&                                   key_,
            const hrbytes&                                   cacheName_,
            uint32_t                                         topologyId_,
            uint32_t                                   flags_,
            const hrbytes&                                   value_,
            uint32_t                                         lifespan_,
            uint32_t                                         maxIdle_)
            : AbstractKeyOperation<T>(codec_, transportFactory_, key_,
                                     cacheName_,
                                      topologyId_, flags_),
            value(value_), lifespan(lifespan_), maxIdle(maxIdle_)
        {}

        const hrbytes& value;
        uint32_t lifespan;
        uint32_t maxIdle;

        //[header][key length][key][lifespan][max idle][value length][value]
        uint8_t sendPutOperation(
            transport::Transport&     transport,
            uint8_t                                       opCode,
            uint8_t                                       /*opRespCode*/)
        {
            // 1) write header
            hr_scoped_ptr<protocol::HeaderParams> params(&(AbstractKeyOperation<T>::writeHeader(transport, opCode)));

            // 2) write key and value
            transport.writeArray(AbstractKeyOperation<T>::key);
            transport.writeVInt(lifespan);
            transport.writeVInt(maxIdle);
            transport.writeArray(value);
            transport.flush();

            // 3) now read header

            //return status (not error status for sure)
            return AbstractKeyOperation<T>::readHeaderAndValidate(transport, *params);
        }
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_ABSTRACTKEYVALUEOPERATION_H
