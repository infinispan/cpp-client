#ifndef ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
#define ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H

/*
 * JBoss, Home of Professional Open Source
 * Copyright 2010 Red Hat Inc. and/or its affiliates and other
 * contributors as indicated by the @author tags. All rights reserved.
 * See the copyright.txt in the distribution for a full listing of
 * individual contributors.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */

#include <string>
#include <vector>
#include "infinispan/hotrod/ImportExport.h"
#include "Configuration.h"
#include "hotrod/impl/configuration/Builder.h"
#include "ConnectionPoolConfigurationBuilder.h"
#include "ServerConfigurationBuilder.h"
#include "SslConfigurationBuilder.h"

namespace infinispan {
namespace hotrod {


class HR_EXTERN ConfigurationBuilder
    : public infinispan::hotrod::Builder<Configuration>
{
  public:
    ConfigurationBuilder();
    ServerConfigurationBuilder& addServer();
    ConfigurationBuilder& addServers(std::string servers);
    ConnectionPoolConfigurationBuilder& connectionPool();
    ConfigurationBuilder& connectionTiemout(int connectionTiemoutParam);
    ConfigurationBuilder& forceReturnValues(bool forceReturnValuesParam);
    ConfigurationBuilder& keySizeEstimate(int keySizeEstimateParam);
    ConfigurationBuilder& pingOnStartup(bool pingOnStartupParam);
    ConfigurationBuilder& protocolVersion(std::string protocolVersionParam);
    ConfigurationBuilder& socketTimeout(int socketTimeoutParam);
    SslConfigurationBuilder& ssl();
    ConfigurationBuilder& tcpNoDelay(bool tcpNoDelayParam);
    ConfigurationBuilder& valueSizeEstimate(int valueSizeEstimateParam);
    Configuration build();
    virtual Configuration create();
    virtual ConfigurationBuilder& read(Configuration& bean);
    static const char* PROTOCOL_VERSION_12;


  private:
    ConnectionPoolConfigurationBuilder connectionPoolConfigurationBuilder;
    int internalConnectionTimeout;
    bool internalForceReturnValue;
    int internalKeySizeEstimate;
    bool internalPingOnStartup;
    std::string internalProtocolVersion;
    std::vector<ServerConfigurationBuilder*> internalServers;
    int internalSocketTimeout;
    SslConfigurationBuilder sslConfigurationBuilder;
    bool internalTcpNoDelay;
    int internalValueSizeEstimate;

};

}} // namespace

#endif // ISPN_HOTROD_CONFIGURATION_CONFIGURATIONBUILDER_H
