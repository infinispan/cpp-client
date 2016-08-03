/*
 * ClientListener.h
 *
 *  Created on: Aug 3, 2016
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_CLIENTLISTENER_H_
#define INCLUDE_INFINISPAN_HOTROD_CLIENTLISTENER_H_
#include <vector>

struct ClientListener
{
	std::vector<char> filterFactoryName;
	std::vector<char> converterFactoryName;
	bool useRawData = false ;
	bool includeCurrentState = false;
};



#endif /* INCLUDE_INFINISPAN_HOTROD_CLIENTLISTENER_H_ */
