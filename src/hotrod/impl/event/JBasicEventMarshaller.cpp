/*
 * JBasicEventMarshaller.cpp
 *
 *  Created on: Aug 2, 2016
 *      Author: rigazilla
 */

#include <infinispan/hotrod/JBasicEventMarshaller.h>

namespace infinispan {
namespace hotrod {
namespace event {

JBasicEventMarshaller::JBasicEventMarshaller() {
	// TODO Auto-generated constructor stub

}

JBasicEventMarshaller::~JBasicEventMarshaller() {
	// TODO Auto-generated destructor stub
}

void JBasicEventMarshaller::unmarshall(std::vector<char>&, std::string&)
{
	// TODO implements
}
void JBasicEventMarshaller::unmarshall(std::vector<char>&, int&)
{
	// TODO implements
}

std::vector<char> JBasicEventMarshaller::marshall(std::string)
{
	// TODO implements
	return std::vector<char>();
}

std::vector<char> JBasicEventMarshaller::marshall(int)
{
	// TODO implements
	return std::vector<char>();
}


} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */
