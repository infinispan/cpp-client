#ifndef ISPN_HOTROD_PROTOCOL_CODEC_H
#define ISPN_HOTROD_PROTOCOL_CODEC_H

#include <infinispan/hotrod/ClientListener.h>
#include "infinispan/hotrod/defs.h"
#include <set>
#include <map>
#include <vector>
namespace infinispan {
namespace hotrod {

namespace transport {

class Transport;
class InetSocketAddress;
}

using namespace infinispan::hotrod::event;
namespace protocol {

class HeaderParams;

class Codec
{
  public:
    virtual HeaderParams& writeHeader(
      transport::Transport& transport, HeaderParams& params) const = 0;

    virtual uint8_t readHeader(
      transport::Transport& transport, HeaderParams& params) const = 0;

    virtual std::vector<char> returnPossiblePrevValue(transport::Transport& t, uint8_t status, uint32_t flags) const = 0;
    virtual void writeExpirationParams(transport::Transport& t,uint64_t lifespan, uint64_t maxIdle) const = 0;
//    virtual void writeClientListenerParams(transport::Transport& t, const ClientListener clientListener,
//    		const std::vector<std::vector<char> > &filterFactoryParams, const std::vector<std::vector<char> > &converterFactoryParams) const = 0;
//    virtual char readAddEventListenerResponseType(transport::Transport &transport, uint64_t &messageId) const = 0;
//    virtual void processEvent() const = 0;
//    virtual uint8_t readPartialHeader(transport::Transport &transport, HeaderParams &params, uint8_t receivedOpCode) const = 0;
//    virtual std::vector<char> readEventListenerId(transport::Transport &transport) const = 0;
//    virtual uint8_t readEventIsCustomFlag(transport::Transport &transport) const = 0;


    virtual ~Codec() {}
  protected:

};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC_H
