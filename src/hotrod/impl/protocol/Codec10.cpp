

#include "hotrod/impl/protocol/Codec10.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"

#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <vector>

namespace infinispan {
namespace hotrod {

using transport::Transport;
using transport::InetSocketAddress;
using transport::TransportFactory;

namespace protocol {

// TODO : multithread management
long Codec10::msgId = 0;

HeaderParams& Codec10::writeHeader(
    Transport& transport, HeaderParams& params) const
{
    return writeHeader(transport, params, HotRodConstants::VERSION_10);
}

HeaderParams& Codec10::writeHeader(
    Transport& transport, HeaderParams& params, uint8_t version) const
{
    transport.writeByte(HotRodConstants::REQUEST_MAGIC);
    // TODO : multithread management
    transport.writeVLong(params.setMessageId(++msgId).messageId);
    transport.writeByte(version);
    transport.writeByte(params.opCode);
    transport.writeArray(params.cacheName);
    transport.writeVInt(params.flags);
    transport.writeByte(params.clientIntel);
    transport.writeVInt(params.topologyId.get());
    transport.writeByte(params.txMarker);
    return params;
}

uint8_t Codec10::readHeader(
    Transport& transport, HeaderParams& params) const
{
    uint8_t magic = transport.readByte();
    if (magic != HotRodConstants::RESPONSE_MAGIC) {
        std::ostringstream message;
        message << "Invalid magic number. Expected " <<
            HotRodConstants::RESPONSE_MAGIC << " and received " << magic;
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
        checkForErrorsInResponseStatus(transport, params, status);
        }
        std::ostringstream message;
        message << "Invalid response operation. Expected " << std::hex <<
            (int) params.opRespCode << " and received " << std::hex << (int) receivedOpCode;
        throw InvalidResponseException(message.str());
    }

    return status;
}

void Codec10::readNewTopologyIfPresent(
    Transport& transport, HeaderParams& params) const
{
    uint8_t topologyChangeByte = transport.readByte();
    if (topologyChangeByte == 1)
        readNewTopologyAndHash(transport, params.topologyId);
}

void Codec10::readNewTopologyAndHash(Transport& transport, IntWrapper& topologyId) const{
    uint32_t newTopologyId = transport.readVInt();
    topologyId.set(newTopologyId); //update topologyId reference
    int16_t numKeyOwners = transport.readUnsignedShort();
    uint8_t hashFunctionVersion = transport.readByte();
    uint32_t hashSpace = transport.readVInt();
    uint32_t clusterSize = transport.readVInt();
    std::map<InetSocketAddress, std::set<int32_t> > m = computeNewHashes(
            transport, newTopologyId, numKeyOwners, hashFunctionVersion, hashSpace,
            clusterSize);

    std::vector<InetSocketAddress> socketAddresses;
    for (std::map<InetSocketAddress, std::set<int32_t> >::iterator it =
            m.begin(); it != m.end(); ++it) {
        socketAddresses.push_back(it->first);
    }
    transport.getTransportFactory().updateServers(socketAddresses);

    if (hashFunctionVersion == 0) {
        //TODO log there was no hash function present
    } else {
        transport.getTransportFactory().updateHashFunction(m, numKeyOwners,
                hashFunctionVersion, hashSpace);
    }
}

std::map<InetSocketAddress, std::set<int32_t> > Codec10::computeNewHashes(
        Transport& transport, uint32_t /*newTopologyId*/, int16_t /*numKeyOwners*/,
        uint8_t /*hashFunctionVersion*/, uint32_t /*hashSpace*/, uint32_t clusterSize) const {

    std::map<InetSocketAddress, std::set<int32_t> > map;
    for (uint32_t i = 0; i < clusterSize; i++) {
        std::string host = transport.readString();
        int16_t port = transport.readUnsignedShort();
        int32_t hashCode = transport.read4ByteInt();
        InetSocketAddress address(host, port);

        std::map<InetSocketAddress, std::set<int32_t> >::iterator it =
                map.find(address);
        if (it == map.end()) {
            std::set<int32_t> hashes;
            hashes.insert(hashCode);
            map.insert(
                    std::pair<InetSocketAddress, std::set<int32_t> >(address,
                            hashes));
        } else {
            it->second.insert(hashCode);
        }
    }
    return map;
}

void Codec10::checkForErrorsInResponseStatus(Transport& transport, HeaderParams& params, uint8_t status) const {
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
                throw RemoteNodeSuspectException(msgFromServer, params.messageId, status);
            } else {
                throw HotRodClientException(msgFromServer); //, params.messageId, status);
            }
        }
        default: {
            throw InternalException("Unknown status: " + status);
        }
        }
    } catch (Exception const& e) {
        // Some operations require invalidating the transport
        switch (status) {
        case HotRodConstants::INVALID_MAGIC_OR_MESSAGE_ID_STATUS:
        case HotRodConstants::REQUEST_PARSING_ERROR_STATUS:
        case HotRodConstants::UNKNOWN_COMMAND_STATUS:
        case HotRodConstants::UNKNOWN_VERSION_STATUS: {
            transport.invalidate();
        }
        }
        throw e; // rethrow
    }
}

}}} // namespace infinispan::hotrod::protocol
