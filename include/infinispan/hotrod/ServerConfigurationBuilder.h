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
class ServerConfigurationBuilder
  : public Builder<ServerConfiguration>, public ConfigurationChildBuilder
{
  public:
    ServerConfigurationBuilder(ConfigurationBuilder& builder_):
        ConfigurationChildBuilder(builder_), m_host("localhost"), m_port(11222) {}

	/***
	 * Specifies host of remote HotRod server
	 *
	 * \return ServerConfigurationBuilder for further configuration
	 */
    ServerConfigurationBuilder& host(const std::string &host_)
    {
        m_host = host_;
        return *this;
    }

    /***
    	 * Specifies port of remote HotRod server
    	 *
    	 * \return ServerConfigurationBuilder for further configuration
    	 */
    ServerConfigurationBuilder& port(int port_)
    {
        m_port = port_;
        return *this;
    }

	/***
	 * Create ServerConfiguration instance from a given state of
	 * this ServerConfigurationBuilder
	 *
	 * \return created ServerConfiguration instance
	 */
    virtual ServerConfiguration create()
    {
        return ServerConfiguration(m_host, m_port);
    }

	/***
	 * Create ServerConfigurationBuilder from a given state of
	 * another ServerConfiguration instance
	 *
	 * \return ServerConfigurationBuilder for further configuration
	 */
    virtual ServerConfigurationBuilder& read(ServerConfiguration& configuration)
    {
        m_host = configuration.getHost();
        m_port = configuration.getPort();
        return *this;
    }

  private:
    std::string m_host;
    int m_port;
};


}} //namespace


#endif /* SERVERCONFIGURATIONBUILDER_H_ */
