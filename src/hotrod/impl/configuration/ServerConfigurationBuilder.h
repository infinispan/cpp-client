/*
 * ServerConfigurationBuilder.h
 *
 *  Created on: Jul 18, 2013
 *      Author: samuele
 */

#ifndef SERVERCONFIGURATIONBUILDER_H_
#define SERVERCONFIGURATIONBUILDER_H_

#include <string>
#include "infinispan/hotrod/ImportExport.h"
#include "hotrod/impl/configuration/Builder.h"
#include "hotrod/impl/configuration/ServerConfiguration.h"

namespace infinispan {
namespace hotrod {

class HR_EXTERN ServerConfigurationBuilder
  : public Builder<ServerConfiguration>
{
  public:
    ServerConfigurationBuilder& host(std::string hostParam);
    ServerConfigurationBuilder& port(int portParam);
    virtual ServerConfiguration create();
    virtual ServerConfigurationBuilder& read(ServerConfiguration& bean);

  private:
    std::string internalHost;
    int internalPort;
};


}} //namespace


#endif /* SERVERCONFIGURATIONBUILDER_H_ */
