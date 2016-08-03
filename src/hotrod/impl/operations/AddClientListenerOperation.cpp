/*
 * AddClientListenerOperation.cpp
 *
 *  Created on: Aug 2, 2016
 *      Author: rigazilla
 */

#include <hotrod/impl/operations/AddClientListenerOperation.h>
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
    codec.writeClientListenerParams(transport, clientListener, filterFactoryParams, converterFactoryParams);
    return 0;

}


} /* namespace operations */
} /* namespace hotrod */
} /* namespace infinispan */
