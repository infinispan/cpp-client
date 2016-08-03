/*
 * ClientListenerNotifier.h
 *
 *  Created on: Aug 2, 2016
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_EVENT_CLIENTLISTENERNOTIFIER_H_
#define SRC_HOTROD_IMPL_EVENT_CLIENTLISTENERNOTIFIER_H_

namespace infinispan {
namespace hotrod {
namespace event {

class ClientListenerNotifier {
public:
	ClientListenerNotifier();
	virtual ~ClientListenerNotifier();
};

} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_EVENT_CLIENTLISTENERNOTIFIER_H_ */
