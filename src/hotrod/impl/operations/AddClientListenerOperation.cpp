/*
 * AddClientListenerOperation.cpp
 *
 *  Created on: Aug 2, 2016
 *      Author: rigazilla
 */

#include <hotrod/impl/operations/AddClientListenerOperation.h>
#include <random>
#include <sstream>
#include <hotrod/impl/transport/tcp/TcpTransport.h>


namespace infinispan {
namespace hotrod {
namespace operations {



/* This function generate an random ID that seems a V4 UUID
 * it's not an implementation of UUID v4
 */
std::vector<char> AddClientListenerOperation::generateV4UUID()
{
	std::vector<char> tmp(16);
	static std::default_random_engine e{};
	static std::uniform_int_distribution<int> d{0,255};
	auto i=0;
	for (; i<16; i++)
	{
		tmp[i]=(unsigned char)d(e);
	}
	tmp[6] = (tmp[6] & 0x0F) | 0x40;
	tmp[8] = (tmp[8] & 0x3F) | 0x80;
	return tmp;
}

void AddClientListenerOperation::releaseTransport(transport::Transport* )
{
}
void AddClientListenerOperation::invalidateTransport(const infinispan::hotrod::transport::InetSocketAddress&, transport::Transport*){
}

transport::Transport& AddClientListenerOperation::getTransport(int, const std::set<transport::InetSocketAddress>& failedServers) {
    dedicatedTransport = &transportFactory->getTransport(this->cacheName, failedServers);
    return *dedicatedTransport;
}

//
static void processImmediateEvent(const ClientListener &clientListener, const Codec20& codec20,
		uint8_t respOpCode, transport::Transport& transport) {
	std::vector<char> listId = codec20.readEventListenerId(transport);
	uint8_t isCustom = codec20.readEventIsCustomFlag(transport);
	uint8_t isRetried = codec20.readEventIsRetriedFlag(transport);
	if (isCustom != 0) {
		ClientCacheEntryCustomEvent ev = codec20.readCustomEvent(transport,
				isRetried);
		clientListener.processEvent(ev, listId, isCustom);
	} else {
		switch (respOpCode) {
		case HotRodConstants::CACHE_ENTRY_CREATED_EVENT_RESPONSE: {
			ClientCacheEntryCreatedEvent<std::vector<char> > marshEvent =
					codec20.readCreatedEvent(transport, isRetried);
			clientListener.processEvent(marshEvent, listId, isCustom);
		}
			break;
		case HotRodConstants::CACHE_ENTRY_MODIFIED_EVENT_RESPONSE: {
			ClientCacheEntryModifiedEvent<std::vector<char> > marshEvent =
					codec20.readModifiedEvent(transport, isRetried);
			clientListener.processEvent(marshEvent, listId, isCustom);
		}
			break;
		case HotRodConstants::CACHE_ENTRY_REMOVED_EVENT_RESPONSE: {
			ClientCacheEntryRemovedEvent<std::vector<char> > marshEvent =
					codec20.readRemovedEvent(transport, isRetried);
			clientListener.processEvent(marshEvent, listId, isCustom);
		}
			break;
		case HotRodConstants::CACHE_ENTRY_EXPIRED_EVENT_RESPONSE:
			break;
		default:
			break;
		}
	}
}

char AddClientListenerOperation::executeOperation(transport::Transport& transport)
{
    std::unique_ptr<protocol::HeaderParams> params(this->writeHeader(transport, ADD_CLIENT_LISTENER_REQUEST));
    const Codec20& codec20 = dynamic_cast<const Codec20&>(codec);
    transport.writeArray(listenerId);
    codec20.writeClientListenerParams(transport, clientListener, filterFactoryParams, converterFactoryParams);
    codec20.writeClientListenerInterests(transport, this->clientListener.interestFlag);
    transport.flush();
    listenerNotifier.addClientListener(listenerId, clientListener, cacheName, transport, codec20, (std::shared_ptr<void>)this->shared_from_this(), recoveryCallback);
    this->shared=true;
    const_cast <ClientListener&>(clientListener).setListenerId(listenerId);
    uint64_t respMessageId = 0;
    try
    {
		uint8_t status = NO_ERROR_STATUS;
    	uint8_t respOpCode = codec20.readAddEventListenerResponseType(transport, respMessageId);
    	while (isEvent(respOpCode))
    	{    	// The response contains immediate event to process
			EventHeaderParams params;
			params.messageId=respMessageId;
			params.opCode=respOpCode;
			try
			{
				status = codec20.readPartialEventHeader(transport, params);
				if (HotRodConstants::isSuccess(status))
				{
					processImmediateEvent(clientListener, codec20, respOpCode, transport);
				}
			}
			catch (HotRodClientException e) {
			}
	    	respOpCode = codec20.readAddEventListenerResponseType(transport, respMessageId);
    	}
        if (respMessageId != params->getMessageId() && respMessageId != 0) {
            std::ostringstream message;
            message << "Invalid message id. Expected "
                << params->getMessageId() << " and received "
                << respMessageId;
            throw InvalidResponseException(message.str());
        }
        if (HotRodConstants::isSuccess(status)
                && HotRodConstants::isSuccess(codec20.readPartialHeader(transport, *params,respOpCode)))
    	{
			listenerNotifier.startClientListener(listenerId);
		} else {
			listenerNotifier.removeClientListener(listenerId);
		}
    }
    catch (const TransportException& ex)
    {
        transport::TcpTransport& tcpT = dynamic_cast<transport::TcpTransport&>(transport);
        TransportException tex(tcpT.getServerAddress().getHostname(), tcpT.getServerAddress().getPort(), ex.what(), ex.getErrnum());
    	listenerNotifier.removeClientListener(listenerId);
    	throw tex;
    } 
    catch (const HotRodClientException& hrex)
    {
        listenerNotifier.removeClientListener(listenerId);
        throw hrex;
    }
    catch (const Exception& ex)
    {
        listenerNotifier.removeClientListener(listenerId);
        throw ex;
    }
    return 0;
}


} /* namespace operations */
} /* namespace hotrod */
} /* namespace infinispan */
