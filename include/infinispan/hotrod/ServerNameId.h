/*
 * ServerNameId.h
 *
 *  Created on: Feb 22, 2016
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_SERVERNAMEID_H_
#define INCLUDE_INFINISPAN_HOTROD_SERVERNAMEID_H_

#include <string>
namespace infinispan {
namespace hotrod {

class ServerNameId {
public:
	std::string name;
	virtual ~ServerNameId() {};
	ServerNameId() : name(std::string()) {};
	ServerNameId(std::string s) : name(s) {};
	bool operator < (const ServerNameId& sni) const { return name.compare(sni.name) < 0; };
};

} /* namespace hotrod */
} /* namespace infinispan */

#endif /* INCLUDE_INFINISPAN_HOTROD_SERVERNAMEID_H_ */
