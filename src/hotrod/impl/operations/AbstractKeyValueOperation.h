#ifndef ISPN_HOTROD_OPERATIONS_ABSTRACTKEYVALUEOPERATION_H
#define ISPN_HOTROD_OPERATIONS_ABSTRACTKEYVALUEOPERATION_H



#include "hotrod/impl/operations/AbstractKeyOperation.h"

#include <set>

namespace infinispan {
namespace hotrod {
class Topology;
namespace operations {

template<class T> class AbstractKeyValueOperation : public AbstractKeyOperation<T>
{
    protected:
	    AbstractKeyValueOperation(
            const protocol::Codec&       codec_,
            std::shared_ptr<transport::TransportFactory> transportFactory_,
            const std::vector<char>&                                   key_,
            const std::vector<char>&                                   cacheName_,
            Topology&                                 topologyId_,
            uint32_t                                         flags_,
            const std::vector<char>&                                   value_,
            uint32_t                                         lifespan_,
            uint32_t                                         maxIdle_)
            : AbstractKeyOperation<T>(codec_, transportFactory_, key_,
                                     cacheName_,
                                      topologyId_, flags_),
            value(value_), lifespan(lifespan_), maxIdle(maxIdle_)
        {}

        const std::vector<char>& value;
        uint32_t lifespan;
        uint32_t maxIdle;
        using HotRodOperation<T>::codec;
        //[header][key length][key][lifespan][max idle][value length][value]
        uint8_t sendPutOperation(
            transport::Transport&     transport,
            uint8_t                                       opCode,
            uint8_t                                       /*opRespCode*/)
        {
            // 1) write header
            std::unique_ptr<protocol::HeaderParams> params(&(this->writeHeader(transport, opCode)));

            // 2) write key and value
            transport.writeArray(this->key);
            codec.writeExpirationParams(transport, lifespan, maxIdle);
            transport.writeArray(value);
            transport.flush();

            // 3) now read header

            //return status (not error status for sure)
            return this->readHeaderAndValidate(transport, *params);
        }
};

}}} // namespace infinispan::hotrod::operations

#endif // ISPN_HOTROD_OPERATIONS_ABSTRACTKEYVALUEOPERATION_H
