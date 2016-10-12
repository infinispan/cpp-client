#ifndef ISPN_HOTROD_PROTOCOL_CODEC20_H
#define ISPN_HOTROD_PROTOCOL_CODEC20_H

#include "hotrod/impl/protocol/Codec.h"

namespace infinispan {
namespace hotrod {
class Topology;

namespace transport {
class Transport;
}

namespace event {
class EventHeaderParams;
}

namespace protocol {

class HeaderParams;

class Codec20 : public Codec
{
  public:
    HeaderParams& writeHeader(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params) const;

    uint8_t readHeader(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params) const;

    long getMessageId();

    std::vector<char> returnPossiblePrevValue(transport::Transport& t, uint8_t status, uint32_t flags) const;
    void writeExpirationParams(transport::Transport& t,uint64_t lifespan, uint64_t maxIdle) const;
    virtual void writeClientListenerParams(transport::Transport& t, const ClientListener& clientListener,
    		const std::vector<std::vector<char> > &filterFactoryParams, const std::vector<std::vector<char> > &converterFactoryParams) const;
    void writeNamedFactory(transport::Transport &transport, const std::vector<char> &factoryName, const std::vector<std::vector<char> > & params) const;
    virtual char readAddEventListenerResponseType(transport::Transport &transport, uint64_t &messageId) const;
    virtual void processEvent() const;
    virtual uint8_t readPartialHeader(transport::Transport &transport, HeaderParams &params, uint8_t receivedOpCode) const;
    virtual uint8_t readPartialEventHeader(transport::Transport &transport, EventHeaderParams &params) const;
    virtual std::vector<char> readEventListenerId(transport::Transport &transport) const;
    virtual uint8_t readEventIsCustomFlag(transport::Transport &transport) const;
    virtual uint8_t readEventIsRetriedFlag(transport::Transport &transport) const;
    virtual ClientCacheEntryCustomEvent readCustomEvent(transport::Transport &transport) const;
    virtual ClientCacheEntryExpiredEvent processExpiredEvent(transport::Transport &transport) const;
    virtual ClientCacheEntryCreatedEvent<std::vector<char>> readCreatedEvent(transport::Transport &transport, uint8_t isRetried) const;
    virtual ClientCacheEntryModifiedEvent<std::vector<char>> readModifiedEvent(transport::Transport &transport, uint8_t isRetried) const;
    virtual ClientCacheEntryRemovedEvent<std::vector<char>> readRemovedEvent(transport::Transport &transport, uint8_t isRetried) const;

    virtual event::EventHeaderParams readEventHeader(transport::Transport& transport) const;
  protected:
    HeaderParams& writeHeader(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params, uint8_t version) const;

    Codec20() {}

  friend class CodecFactory;

  private:

    void readNewTopologyIfPresent(
        infinispan::hotrod::transport::Transport& transport,
        HeaderParams& params) const;

    void readNewTopologyAndHash(infinispan::hotrod::transport::Transport& transport, HeaderParams& params) const;

    void checkForErrorsInResponseStatus(
        infinispan::hotrod::transport::Transport& transport,
        uint64_t messageId, uint8_t status) const;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC20_H
