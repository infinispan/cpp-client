/*
 * ServerConfiguration.h
 *
 *  Created on: Jul 10, 2013
 *      Author: samuele
 */

#ifndef SERVERCONFIGURATION_H_
#define SERVERCONFIGURATION_H_

#include <string>
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

    ServerConfiguration(std::string host,
        int port);

	/**
	 * Returns host of this ServerConfiguration
	 *
	 * \return host as a string reference
	 */
    const std::string& getHost() const;

    /**
    	 * Returns port of this ServerConfiguration
    	 *
    	 * \return port as an in reference
    	 */
    const int& getPort() const;

  private:
    std::string host;
    int port;
};

}
}
#endif /* SERVERCONFIGURATION_H_ */
