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


class HR_EXTERN ServerConfiguration
{
  public:

    ServerConfiguration(std::string host,
        int port);

    const std::string& getHost() const;
    const int& getPort() const;

  private:
    std::string host;
    int port;
};

}
}
#endif /* SERVERCONFIGURATION_H_ */
