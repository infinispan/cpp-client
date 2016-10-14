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

transport::Transport& AddClientListenerOperation::getTransport(int, const std::set<transport::InetSocketAddress>& failedServers)
{
	dedicatedTransport= transportFactory->getTransport(this->cacheName, failedServers).clone();
	return *dedicatedTransport;
}

char AddClientListenerOperation::executeOperation(transport::Transport& transport)
{
    protocol::HeaderParams params = this->writeHeader(transport, ADD_CLIENT_LISTENER_REQUEST);
    transport.writeArray(listenerId);
    const Codec20& codec20 = dynamic_cast<const Codec20&>(codec);
    codec20.writeClientListenerParams(transport, clientListener, filterFactoryParams, converterFactoryParams);
    transport.flush();
    listenerNotifier.addClientListener(listenerId, clientListener, cacheName, transport, codec20, (void*)this, recoveryCallback);
    const_cast <ClientListener&>(clientListener).setListenerId(listenerId);
    bool readMore = true;
    uint64_t respMessageId = 0;
    try
    {
    do
    {
    	uint8_t respOpCode = codec20.readAddEventListenerResponseType(transport, respMessageId);
    	// The response contains immediate event to process
    	if (isEvent(respOpCode))
    	{
			EventHeaderParams params;
			params.messageId=respMessageId;
			params.opCode=respOpCode;
			try
			{
				uint8_t status = codec20.readPartialEventHeader(transport, params);
				if (!HotRodConstants::isSuccess(status))
					break;
			}
			catch (HotRodClientException e) {
				continue;
			}
		    std::vector<char> listId=codec20.readEventListenerId(transport);
		    uint8_t isCustom = codec20.readEventIsCustomFlag(transport);
		    uint8_t isRetried = codec20.readEventIsRetriedFlag(transport);
		    if (isCustom != 0)
		    {
		    	ClientCacheEntryCustomEvent ev = codec20.readCustomEvent(transport,isRetried);
		    	clientListener.processEvent(ev, listId, isCustom);
		    }
		    else
		    {
				switch (respOpCode) {
				case CACHE_ENTRY_CREATED_EVENT_RESPONSE: {
					ClientCacheEntryCreatedEvent<std::vector<char>> marshEvent =
							codec20.readCreatedEvent(transport, isRetried);
					clientListener.processEvent(marshEvent, listId, isCustom);
				}
					break;
				case CACHE_ENTRY_MODIFIED_EVENT_RESPONSE: {
					ClientCacheEntryModifiedEvent<std::vector<char>> marshEvent =
							codec20.readModifiedEvent(transport, isRetried);
					clientListener.processEvent(marshEvent, listId, isCustom);
				}
					break;
				case CACHE_ENTRY_REMOVED_EVENT_RESPONSE: {
					ClientCacheEntryRemovedEvent<std::vector<char>> marshEvent =
							codec20.readRemovedEvent(transport, isRetried);
					clientListener.processEvent(marshEvent, listId, isCustom);
				}
				break;
				case CACHE_ENTRY_EXPIRED_EVENT_RESPONSE:
					break;
				default:
					break;
				}
			}
    	}
        else
        {
        	if (respMessageId != params.getMessageId() && respMessageId != 0) {
        	    std::ostringstream message;
        	    message << "Invalid message id. Expected " <<
        	        params.getMessageId() << " and received " << respMessageId;
        	    throw InvalidResponseException(message.str());
        	}
        	uint8_t status = codec20.readPartialHeader(transport, params, respOpCode);
        	if (HotRodConstants::isSuccess(status))
        	{
        		listenerNotifier.startClientListener(listenerId);
        	}
        	else
        	{
        		listenerNotifier.removeClientListener(listenerId);
        	}
        	readMore=false;
    	}
    } while (readMore);
    }
    catch (const TransportException& ex)
    {
    	listenerNotifier.removeClientListener(listenerId);
    	transport::TcpTransport& tcpT = dynamic_cast<transport::TcpTransport&>(transport);
    	throw TransportException(tcpT.getServerAddress().getHostname(), tcpT.getServerAddress().getPort(), ex.what(), ex.getErrnum());
    }
    return 0;
}


} /* namespace operations */
} /* namespace hotrod */
} /* namespace infinispan */
