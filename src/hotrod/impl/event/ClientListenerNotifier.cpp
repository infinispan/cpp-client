/*
 * ClientListenerNotifier.cpp
 *
 *  Created on: Aug 2, 2016
 *      Author: rigazilla
 */

#include <hotrod/impl/event/ClientListenerNotifier.h>

namespace infinispan {
namespace hotrod {
namespace event {

ClientListenerNotifier::ClientListenerNotifier() {
	// TODO Auto-generated constructor stub

}

ClientListenerNotifier::~ClientListenerNotifier() {
	// TODO Auto-generated destructor stub
}

ClientListenerNotifier* ClientListenerNotifier::create(Codec &, const EventMarshaller &)
{
	return new ClientListenerNotifier();
}

} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */
