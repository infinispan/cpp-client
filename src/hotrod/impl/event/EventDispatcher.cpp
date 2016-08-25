/*
 * EventDispatcher.cpp
 *
 *  Created on: Aug 19, 2016
 *      Author: rigazilla
 */

#include "hotrod/impl/event/EventDispatcher.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include <thread>
#include <iostream>

using namespace infinispan::hotrod::protocol;
namespace infinispan {
namespace hotrod {
namespace event {

void EventDispatcher::start()
{
  p_thread.reset(new std::thread(&EventDispatcher::run, this));
}

void EventDispatcher::stop()
{
	// TODO terminate the thread before nullify the pointer
  p_thread=nullptr;
}

void EventDispatcher::run() {
   //Thread.currentThread().setName(getThreadName());
	try
	{
      while (1) {
    	    uint64_t messageId;
    	    EventHeaderParams params=codec20.readEventHeader(transport);
        	if (!(HotRodConstants::isEvent(params.opCode))) {
        		// TODO handle error in some way
        		return;
        	}
		    std::vector<char> listId=codec20.readEventListenerId(transport);
		    uint8_t isCustom = codec20.readEventIsCustomFlag(transport);
		    uint8_t isRetried = codec20.readEventIsRetriedFlag(transport);

    	  uint8_t magic = transport.readByte();
    	  std::cout << "Hey Gotta Event magic is: " << magic;
    	  //            ClientEvent clientEvent = null;
//            try {
//               clientEvent = codec.readEvent(transport, op.listenerId, marshaller);
//               invokeClientEvent(clientEvent);
//               // Nullify event, makes it easier to identify network vs invocation error messages
//               clientEvent = null;
//            } catch (TransportException e) {
//               Throwable cause = e.getCause();
//               if (cause instanceof ClosedChannelException) {
//                  // Channel closed, ignore and exit
//                  log.debug("Channel closed, exiting event reader thread");
//                  stopped = true;
//                  return;
//               } else if (cause instanceof SocketTimeoutException) {
//                  log.debug("Timed out reading event, retry");
//               } else if (clientEvent != null) {
//                  log.unexpectedErrorConsumingEvent(clientEvent, e);
//               }  else {
//                  log.unrecoverableErrorReadingEvent(e, transport.getRemoteSocketAddress());
//                  stopped = true;
//                  return; // Server is likely gone!
//               }
//            } catch (CancelledKeyException e) {
//               // Cancelled key exceptions are also thrown when the channel has been closed
//               log.debug("Key cancelled, most likely channel closed, exiting event reader thread");
//               stopped = true;
//               return;
//            } catch (Throwable t) {
//               if (clientEvent != null)
//                  log.unexpectedErrorConsumingEvent(clientEvent, t);
//               else
//                  log.unableToReadEventFromServer(t, transport.getRemoteSocketAddress());
//               if (!transport.isValid()) {
//                  stopped = true;
//                  return;
//               }
//            }
   }
} catch (std::exception e)
{

}
}

} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */
