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
/**
 * ServerConfigurationBuilder is a factory for ServerConfiguration instances.
 *
 * Together these two provide configuration of remote HotRod servers C++
 * HotRod clients communicate with.
 *
 */
class HR_EXTERN ServerConfigurationBuilder
  : public Builder<ServerConfiguration>, public ConfigurationChildBuilder
{
  public:
    ServerConfigurationBuilder(ConfigurationBuilder& builder_);

	/***
	 * Specifies host of remote HotRod server
	 *
	 * \return ServerConfigurationBuilder for further configuration
	 */
    ServerConfigurationBuilder& host(std::string hostParam);

    /***
    	 * Specifies port of remote HotRod server
    	 *
    	 * \return ServerConfigurationBuilder for further configuration
    	 */
    ServerConfigurationBuilder& port(int portParam);

	/***
	 * Create ServerConfiguration instance from a given state of
	 * this ServerConfigurationBuilder
	 *
	 * \return created ServerConfiguration instance
	 */
    virtual ServerConfiguration create();

	/***
	 * Create ServerConfigurationBuilder from a given state of
	 * another ServerConfiguration instance
	 *
	 * \return ServerConfigurationBuilder for further configuration
	 */
    virtual ServerConfigurationBuilder& read(ServerConfiguration& bean);

  private:
    std::string m_host;
    int m_port;
};


}} //namespace


#endif /* SERVERCONFIGURATIONBUILDER_H_ */
