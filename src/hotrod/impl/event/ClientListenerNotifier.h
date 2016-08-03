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
namespace operations {
	class AddClientListenerOperation;
}}}


namespace infinispan {
namespace hotrod {
namespace event {

using  namespace infinispan::hotrod::operations;

class ClientListenerNotifier {
public:
	ClientListenerNotifier();
	virtual ~ClientListenerNotifier();
	void addClientListener(AddClientListenerOperation* const) const
	{

	}
};

} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_EVENT_CLIENTLISTENERNOTIFIER_H_ */
