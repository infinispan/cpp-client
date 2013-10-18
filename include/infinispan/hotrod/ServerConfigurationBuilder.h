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
#include "Builder.h"
#include "ServerConfiguration.h"
#include "ConfigurationChildBuilder.h"

namespace infinispan {
namespace hotrod {

class HR_EXTERN ServerConfigurationBuilder
  : public Builder<ServerConfiguration>, public ConfigurationChildBuilder
{
  public:
    ServerConfigurationBuilder(ConfigurationBuilder& builder_);
    ServerConfigurationBuilder& host(std::string hostParam);
    ServerConfigurationBuilder& port(int portParam);
    virtual ServerConfiguration create();
    virtual ServerConfigurationBuilder& read(ServerConfiguration& bean);

  private:
    std::string m_host;
    int m_port;
};


}} //namespace


#endif /* SERVERCONFIGURATIONBUILDER_H_ */
