/*
 * ServerConfiguration.h
 *
 *  Created on: Jul 10, 2013
 *      Author: samuele
 */

#ifndef SERVERCONFIGURATION_H_
#define SERVERCONFIGURATION_H_

#include <string>
#include "infinispan/hotrod/defs.h"
#include "infinispan/hotrod/portable.h"
#include "infinispan/hotrod/ImportExport.h"

namespace infinispan {
namespace hotrod {

/**
 * ServerConfiguration along with its factory ServerConfigurationBuilder provide
 * configuration of remote HotRod servers C++ clients communicate with.
 *
 */
class HR_EXTERN ServerConfiguration
{
public:
    ServerConfiguration(): host(""), port(0) {} // for use just in collections
    ServerConfiguration(const std::string &_host, int _port): host(_host), port(_port) {}

    const std::string &getHost() const
    {
        if (hostPtr.get() == NULL) {
            const_cast<ServerConfiguration *>(this)->hostPtr.set(new std::string(host.c_string()), &deleteString);
        }
        return *hostPtr.get();
    }
	/**
	 * Returns host of this ServerConfiguration
	 *
	 * \return host as a string reference
	 */
    const char *getHostCString() const
    {
        return host.c_string();
    }

    /**
     * Returns port of this ServerConfiguration
     *
     * \return port as an in reference
     */
    const int& getPort() const
    {
        return port;
    }

private:
    portable::string host;
    __pragma(warning(suppress:4251))
    portable::local_ptr<std::string> hostPtr;
    int port;

    static void deleteString(std::string *str) { delete str; }
};

}
}
#endif /* SERVERCONFIGURATION_H_ */
