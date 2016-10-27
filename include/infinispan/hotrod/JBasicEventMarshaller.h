/*
 * JBasicEventMarshaller.h
 *
 *  Created on: Aug 2, 2016
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_JBASICEVENTMARSHALLER_H_
#define INCLUDE_INFINISPAN_HOTROD_JBASICEVENTMARSHALLER_H_

#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/EventMarshaller.h"
#include <vector>
#include <string>
namespace infinispan {
namespace hotrod {
namespace event {

class HR_EXPORT JBasicEventMarshaller: public EventMarshaller {
public:
	JBasicEventMarshaller();
	virtual ~JBasicEventMarshaller();

	virtual void unmarshall(std::vector<char>&, std::string&);
	virtual void unmarshall(std::vector<char>&, int&);

	virtual std::vector<char> marshall(std::string);
	virtual std::vector<char> marshall(int);

};

} /* namespace event */
} /* namespace hotrod */
} /* namespace infinispan */

#endif /* INCLUDE_INFINISPAN_HOTROD_JBASICEVENTMARSHALLER_H_ */
