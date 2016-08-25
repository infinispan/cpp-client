/*
 * AddClientListenerOperation.cpp
 *
 *  Created on: Aug 2, 2016
 *      Author: rigazilla
 */

#include <hotrod/impl/operations/AddCacheClientListenerOperation.h>
#include <random>


namespace infinispan {
namespace hotrod {
namespace operations {



AddClientListenerOperation::~AddClientListenerOperation() {
	// TODO Auto-generated destructor stub
}

/* This function generate an random ID that seems a V4 UUID
 * it's not an implementation of UUID v4
 */
std::vector<char> AddClientListenerOperation::generateV4UUID()
{
	std::vector<char> tmp(16);
	static std::default_random_engine e{};
	static std::uniform_int_distribution<unsigned char> d{0,255};
	auto i=0;
	for (; i<16; i++)
	{
		tmp[i]=d(e);
	}
	tmp[6] = (tmp[6] & 0x0F) | 0x40;
	tmp[8] = (tmp[8] & 0x3F) | 0x80;
	return tmp;
}

void AddClientListenerOperation::releaseTransport(transport::Transport* )
{
}

transport::Transport& AddClientListenerOperation::getTransport(int )
{
	dedicatedTransport= &(this->transportFactory->getTransport(this->cacheName));
	return *dedicatedTransport;
}

transport::Transport& AddClientListenerOperation::getDedicatedTransport()
{
	return *this->dedicatedTransport;
}
char AddClientListenerOperation::executeOperation(transport::Transport& transport)
{
    protocol::HeaderParams params = this->writeHeader(transport, ADD_CLIENT_LISTENER_REQUEST);
    transport.writeArray(listenerId);
    const Codec20& codec20 = dynamic_cast<const Codec20&>(codec);
    codec20.writeClientListenerParams(transport, clientListener, filterFactoryParams, converterFactoryParams);
    transport.flush();
    listenerNotifier.addClientListener(listenerId, clientListener, cacheName, transport, codec20);
    bool readMore = true;
    uint64_t respMessageId = 0;
    do
    {
    	uint8_t respOpCode = codec20.readAddEventListenerResponseType(transport, respMessageId);
    	// The response contains immediate event to process
    	if (isEvent(respOpCode))
    	{
		    std::vector<char> listId=codec20.readEventListenerId(transport);
		    uint8_t isCustom = codec20.readEventIsCustomFlag(transport);
		    uint8_t isRetried = codec20.readEventIsRetriedFlag(transport);
		    if (isCustom != 0)
		    {
		    	ClientCacheEntryCustomEvent ev = codec20.readCustomEvent(transport);
		    }
        	switch (respOpCode)
        	{
        	    case CACHE_ENTRY_CREATED_EVENT_RESPONSE:
        	    {
        	    	ClientCacheEntryCreatedEvent<std::vector<char>> marshEvent = codec20.readCreatedEvent(transport, isRetried);
        	    	clientListener.processEvent(marshEvent, listId, isCustom);
        	    	//clientListener.processEvent(listId, isCustom, isRetried, transport, codec20);
        	    }
    		    break;
        	    case CACHE_ENTRY_EXPIRED_EVENT_RESPONSE:
        	    case CACHE_ENTRY_MODIFIED_EVENT_RESPONSE:
        	    case CACHE_ENTRY_REMOVED_EVENT_RESPONSE:
        	    break;
        	    default:
        	    	break;
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
    return 0;
}


} /* namespace operations */
} /* namespace hotrod */
} /* namespace infinispan */
