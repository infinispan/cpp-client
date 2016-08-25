#include <infinispan/hotrod/InetSocketAddress.h>
#include "infinispan/hotrod/exceptions.h"
#include "hotrod/impl/protocol/Codec20.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/sys/Log.h"
#include "hotrod/sys/Mutex.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <map>
#include <vector>

namespace infinispan {
namespace hotrod {

using transport::Transport;
using event::EventHeaderParams;
using transport::InetSocketAddress;
using transport::TransportFactory;
using infinispan::hotrod::sys::Mutex;
using infinispan::hotrod::sys::ScopedLock;
using infinispan::hotrod::sys::ScopedUnlock;

namespace protocol {

extern long msgId;
extern Mutex lock;

long Codec20::getMessageId() {
    ScopedLock<Mutex> l(lock);
    long msgIdToBeReturned = msgId;
    ScopedUnlock<Mutex> ul(lock);
    return msgIdToBeReturned;
}

HeaderParams& Codec20::writeHeader(
    Transport& transport, HeaderParams& params) const
{
    return writeHeader(transport, params, HotRodConstants::VERSION_20);
}

HeaderParams& Codec20::writeHeader(
    Transport& transport, HeaderParams& params, uint8_t version) const
{
    transport.writeByte(HotRodConstants::REQUEST_MAGIC);
    ScopedLock<Mutex> l(lock);
    transport.writeVLong(params.setMessageId(++msgId).messageId);
    ScopedUnlock<Mutex> ul(lock);
    transport.writeByte(version);
    transport.writeByte(params.opCode);
    transport.writeArray(params.cacheName);
    transport.writeVInt(params.flags);
    transport.writeByte(params.clientIntel);
    transport.writeVInt(params.topologyId.getId());
    return params;
}

EventHeaderParams Codec20::readEventHeader(Transport& transport) const
{
	EventHeaderParams params;
    params.magic = transport.readByte();
    if (params.magic != HotRodConstants::RESPONSE_MAGIC) {
        std::ostringstream message;
        message << std::hex << std::setfill('0');
        message << "Invalid magic number. Expected 0x" << std::setw(2) << static_cast<unsigned>(HotRodConstants::RESPONSE_MAGIC) << " and received 0x" << std::setw(2) << static_cast<unsigned>(params.magic);
        throw InvalidResponseException(message.str());
    }

    params.messageId = transport.readVLong();
    params.opCode = transport.readByte();
    params.status = transport.readByte();
    // No topology change in events, discard the byte
    transport.readByte();

    // Now that all headers values have been read, check the error responses.
    // This avoids situations where an exceptional return ends up with
    // the socket containing data from previous request responses.
      if (params.opCode == HotRodConstants::ERROR_RESPONSE) {
        checkForErrorsInResponseStatus(transport, params.messageId, params.status);
        std::ostringstream message;
        message << "Invalid response operation. Expected " << std::hex <<
            (int) params.opCode << " and received " << std::hex << (int) params.opCode;
        throw InvalidResponseException(message.str());
      }
    return params;
}

uint8_t Codec20::readHeader(
    Transport& transport, HeaderParams& params) const
{
    uint8_t magic = transport.readByte();
    if (magic != HotRodConstants::RESPONSE_MAGIC) {
        std::ostringstream message;
        message << std::hex << std::setfill('0');
        message << "Invalid magic number. Expected 0x" << std::setw(2) << static_cast<unsigned>(HotRodConstants::RESPONSE_MAGIC) << " and received 0x" << std::setw(2) << static_cast<unsigned>(magic);
        throw InvalidResponseException(message.str());
    }

    uint64_t receivedMessageId = transport.readVLong();
    // TODO: java comment, to be checked
    // If received id is 0, it could be that a failure was noted before the
    // message id was detected, so don't consider it to a message id error
    if (receivedMessageId != params.messageId && receivedMessageId != 0) {
        std::ostringstream message;
        message << "Invalid message id. Expected " <<
            params.messageId << " and received " << receivedMessageId;
        throw InvalidResponseException(message.str());
    }

    uint8_t receivedOpCode = transport.readByte();

    // Read both the status and new topology (if present),
    // before deciding how to react to error situations.
    uint8_t status = transport.readByte();
    readNewTopologyIfPresent(transport, params);

    // Now that all headers values have been read, check the error responses.
    // This avoids situations where an exceptional return ends up with
    // the socket containing data from previous request responses.
    if (receivedOpCode != params.opRespCode) {
      if (receivedOpCode == HotRodConstants::ERROR_RESPONSE) {
        checkForErrorsInResponseStatus(transport, params.messageId, status);
        }
        std::ostringstream message;
        message << "Invalid response operation. Expected " << std::hex <<
            (int) params.opRespCode << " and received " << std::hex << (int) receivedOpCode;
        throw InvalidResponseException(message.str());
    }

    return status;
}

void Codec20::readNewTopologyIfPresent(
    Transport& transport, HeaderParams& params) const
{
    uint8_t topologyChangeByte = transport.readByte();
    if (topologyChangeByte == 1)
    {
    	TRACE("Topology has changes");
        readNewTopologyAndHash(transport, params);
    }
    else
    {
    	TRACE("No topology changes");
    }
}

void Codec20::readNewTopologyAndHash(Transport& transport, HeaderParams& params) const{
    // Just consume the header's byte
	// Do not evaluate new topology right now
	int newTopologyId = transport.readVInt();
    params.topologyId.setId(newTopologyId); //update topologyId reference
    uint32_t clusterSize = transport.readVInt();
    TRACE("Coded20::readNewToplogyAndhash(): clusterSize=%d",clusterSize);
    std::vector<InetSocketAddress> addresses(clusterSize);
    for (uint32_t i = 0; i < clusterSize; i++) {
       std::string host(transport.readString());
       int16_t port = transport.readUnsignedShort();
       addresses[i] = InetSocketAddress(host, port);
    }

    uint8_t hashFunctionVersion = transport.readByte();
    uint32_t numSegments = transport.readVInt();

    std::vector<std::vector<InetSocketAddress>> segmentOwners(numSegments);

    if (hashFunctionVersion > 0) {
       TRACE("Codec20::readNewTopologyAndHash: numSegments=%d", numSegments);
       for (uint32_t i = 0; i < numSegments; i++) {
          uint8_t numOwners = transport.readByte();
          segmentOwners[i]=std::vector<InetSocketAddress>(numOwners);
          for (uint8_t j = 0; j < numOwners; j++) {
             uint32_t memberIndex = transport.readVInt();
             segmentOwners[i][j] = addresses[memberIndex];
          }
       }
    }

    TransportFactory &tf = transport.getTransportFactory();
    int currentTopology = tf.getTopologyId(params.cacheName);
    int topologyAge = tf.getTopologyAge();
    if (params.topologyAge == topologyAge && currentTopology != newTopologyId) {
       params.topologyId = newTopologyId;
       tf.updateServers(addresses);
       if (hashFunctionVersion == 0) {
             TRACE("Not using a consistent hash function (hash function version == 0).");
       } else {
             TRACE("Updating client hash function with %u number of segments", numSegments);
       }
       tf.updateHashFunction(segmentOwners,
          numSegments, hashFunctionVersion, params.cacheName, params.topologyId.getId());
    } else {
       TRACE("Outdated topology received (topology id = %d, topology age = %d), so ignoring it: s",
             newTopologyId, topologyAge/*, Arrays.toString(addresses)*/);
    }
}

void Codec20::checkForErrorsInResponseStatus(Transport& transport, uint64_t messageId, uint8_t status) const {
    try {
        switch (status) {
        case HotRodConstants::INVALID_MAGIC_OR_MESSAGE_ID_STATUS:
        case HotRodConstants::REQUEST_PARSING_ERROR_STATUS:
        case HotRodConstants::UNKNOWN_COMMAND_STATUS:
        case HotRodConstants::SERVER_ERROR_STATUS:
        case HotRodConstants::COMMAND_TIMEOUT_STATUS:
        case HotRodConstants::UNKNOWN_VERSION_STATUS: {
            // If error, the body of the message just contains a message
            std::string msgFromServer = transport.readString();
            if (msgFromServer.find("SuspectException") != std::string::npos || msgFromServer.find("SuspectedException") != std::string::npos) {
                // Handle both Infinispan's and JGroups' suspicions
                // TODO: This will be better handled with its own status id in version 2 of protocol
                throw RemoteNodeSuspectException(msgFromServer, messageId, status);
            } else {
                throw HotRodClientException(msgFromServer); //, params.messageId, status);
            }
        }
        default: {
            throw InternalException("Unknown status: " + status);
        }
        }
    } catch (const Exception &) {
        // Some operations require invalidating the transport
        switch (status) {
        case HotRodConstants::INVALID_MAGIC_OR_MESSAGE_ID_STATUS:
        case HotRodConstants::REQUEST_PARSING_ERROR_STATUS:
        case HotRodConstants::UNKNOWN_COMMAND_STATUS:
        case HotRodConstants::UNKNOWN_VERSION_STATUS: {
            transport.invalidate();
        }
        }
        throw;
    }
}
std::vector<char> Codec20::returnPossiblePrevValue(transport::Transport& t, uint8_t status, uint32_t /*flags*/) const{
	std::vector<char> result;
	if (HotRodConstants::hasPrevious(status)) {
		TRACEBYTES("return value = ", result);
		result = t.readArray();
	} else {
		TRACE("No return value");
	}
return result;
}

void Codec20::writeExpirationParams(transport::Transport& t,uint64_t lifespan, uint64_t maxIdle) const {
	uint32_t lInt= (uint32_t) lifespan;
	uint32_t mInt= (uint32_t) maxIdle;
	if ( lInt != lifespan)
		throw HotRodClientException("lifespan exceed 32bit integer");
	if ( mInt != maxIdle)
		throw HotRodClientException("maxIdle exceed 32bit integer");
	t.writeVInt(lInt);
    t.writeVInt(mInt);
}

void Codec20::writeClientListenerParams(transport::Transport& t, const ClientListener& clientListener,
		const std::vector<std::vector<char> > &filterFactoryParams, const std::vector<std::vector<char> > &converterFactoryParams) const
{
    t.writeByte((short)(clientListener.includeCurrentState ? 1 : 0));
    this->writeNamedFactory(t,clientListener.filterFactoryName, filterFactoryParams);
    this->writeNamedFactory(t, clientListener.converterFactoryName, converterFactoryParams);
}

void Codec20::writeNamedFactory(transport::Transport &t, const std::vector<char> &factoryName, const std::vector<std::vector<char> > & params) const
{
   t.writeArray(factoryName);
   if (!factoryName.empty()) {
      // A named factory was written, how many parameters?
      if (!params.empty()) {
         t.writeByte((short) params.size());
         for (auto item: params)
            t.writeArray(item);
      } else {
         t.writeByte((short) 0);
      }
   }
}

char Codec20::readAddEventListenerResponseType(transport::Transport &transport, uint64_t &messageId) const
{
    uint8_t magic = transport.readByte();
    if (magic != HotRodConstants::RESPONSE_MAGIC) {
        std::ostringstream message;
        message << std::hex << std::setfill('0');
        message << "Invalid magic number. Expected 0x" << std::setw(2) << static_cast<unsigned>(HotRodConstants::RESPONSE_MAGIC) << " and received 0x" << std::setw(2) << static_cast<unsigned>(magic);
        throw InvalidResponseException(message.str());
    }
    uint64_t receivedMessageId = transport.readVLong();
    uint8_t receivedOpCode = transport.readByte();
    messageId=receivedMessageId;
    return receivedOpCode;
}

std::vector<char> Codec20::readEventListenerId(transport::Transport &transport) const
{
    return transport.readArray();
}

uint8_t Codec20::readEventIsCustomFlag(transport::Transport &transport) const
{
    return transport.readByte();
}

uint8_t Codec20::readEventIsRetriedFlag(transport::Transport &transport) const
{
    return transport.readByte();
}


ClientCacheEntryCustomEvent Codec20::readCustomEvent(transport::Transport &transport) const
{
	ClientCacheEntryCustomEvent e(transport.readArray());
	return e;
}

ClientCacheEntryExpiredEvent Codec20::processExpiredEvent(transport::Transport &transport) const
{
	ClientCacheEntryExpiredEvent e;
	return e;
}
ClientCacheEntryModifiedEvent Codec20::processModifiedEvent(transport::Transport &transport) const
{
	ClientCacheEntryModifiedEvent e;
	return e;
}

ClientCacheEntryCreatedEvent<std::vector<char>> Codec20::readCreatedEvent(transport::Transport &transport, uint8_t isRetried) const
{
	return ClientCacheEntryCreatedEvent<std::vector<char>>(transport.readArray(), transport.readLong(), isRetried);
}

void Codec20::processEvent() const
{
  //TODO implement
}

uint8_t Codec20::readPartialHeader(transport::Transport &transport, HeaderParams &params, uint8_t receivedOpCode) const
{
    uint8_t status = transport.readByte();
    readNewTopologyIfPresent(transport, params);

    // Now that all headers values have been read, check the error responses.
    // This avoids situations where an exceptional return ends up with
    // the socket containing data from previous request responses.
    if (receivedOpCode != params.opRespCode) {
      if (receivedOpCode == HotRodConstants::ERROR_RESPONSE) {
        checkForErrorsInResponseStatus(transport, params.messageId, status);
        }
        std::ostringstream message;
        message << "Invalid response operation. Expected " << std::hex <<
            (int) params.opRespCode << " and received " << std::hex << (int) receivedOpCode;
        throw InvalidResponseException(message.str());
    }

    return status;
}

}}} // namespace infinispan::hotrod::protocol
