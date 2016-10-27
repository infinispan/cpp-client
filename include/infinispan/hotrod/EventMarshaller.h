/*
 * MarshallerHelper.h
 *
 *  Created on: Aug 2, 2016
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_EVENTMARSHALLER_H_
#define INCLUDE_INFINISPAN_HOTROD_EVENTMARSHALLER_H_
#include <vector>
#include <string>

namespace infinispan {
namespace hotrod {
namespace event {

class EventMarshaller
{
public:
	virtual void unmarshall(std::vector<char>&, std::string&) = 0;
	virtual void unmarshall(std::vector<char>&, int&) = 0;

	virtual std::vector<char> marshall(std::string) = 0;
	virtual std::vector<char> marshall(int) = 0;

	virtual ~EventMarshaller() {};
};


}}}





#endif /* INCLUDE_INFINISPAN_HOTROD_EVENTMARSHALLER_H_ */
