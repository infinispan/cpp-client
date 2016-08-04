/*
 * ClientListenerNotifier.h
 *
 *  Created on: Aug 2, 2016
 *      Author: rigazilla
 */

#ifndef SRC_HOTROD_IMPL_EVENT_CLIENTLISTENERNOTIFIER_H_
#define SRC_HOTROD_IMPL_EVENT_CLIENTLISTENERNOTIFIER_H_

#include "hotrod/impl/protocol/Codec.h"
#include "infinispan/hotrod/EventMarshaller.h"

namespace infinispan {
namespace hotrod {
namespace operations {
	class AddClientListenerOperation;
}}}


namespace infinispan {
namespace hotrod {
namespace event {

using  namespace infinispan::hotrod::operations;
using  namespace infinispan::hotrod::protocol;


class ClientListenerNotifier {
public:
	virtual ~ClientListenerNotifier();
	void addClientListener(AddClientListenerOperation* const) const
	{

	}
	static ClientListenerNotifier* create(Codec &, const EventMarshaller &);

protected:
	ClientListenerNotifier();
};

} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */

#endif /* SRC_HOTROD_IMPL_EVENT_CLIENTLISTENERNOTIFIER_H_ */
