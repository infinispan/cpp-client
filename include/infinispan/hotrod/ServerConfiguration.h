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
        return host;
    }
	/**
	 * Returns host of this ServerConfiguration
	 *
	 * \return host as a string reference
	 */
    const char *getHostCString() const
    {
        return host.data();
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
    std::string host;
    __pragma(warning(suppress:4251))
    int port;

    static void deleteString(std::string *str) { delete str; }
};

}
}
#endif /* SERVERCONFIGURATION_H_ */
