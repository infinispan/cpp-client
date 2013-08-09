#ifndef ISPN_HOTROD_CONFIGURATION_H
#define ISPN_HOTROD_CONFIGURATION_H

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
#include "ConnectionPoolConfiguration.h"
#include "ServerConfiguration.h"
#include "SslConfiguration.h"

namespace infinispan {
namespace hotrod {

class HR_EXTERN Configuration
{
  public:
    // TODO: spostare (java ConfigurationProperties)
    static const char* PROTOCOL_VERSION_12;

    Configuration(std::string protocolVersion,
            const ConnectionPoolConfiguration& connectionPoolConfiguration,
            int connectionTimeout,
            bool forceReturnValue,
            int keySizeEstimate,
            bool pingOnStartup,
            std::vector<ServerConfiguration> serversConfiguration,
            int socketTimeout,
            SslConfiguration sslConfiguration,
            bool tcpNoDelay,
            int valueSizeEstimate);

    const std::string& getProtocolVersion() const;
    const ConnectionPoolConfiguration& getConnectionPoolConfiguration() const;
    const int& getConnectionTimeout() const;
    const bool& isForceReturnValue() const;
    const int& getKeySizeEstimate() const;
    const bool& isPingOnStartup() const;
    const std::vector<ServerConfiguration>& getServersConfiguration() const;
    const int& getSocketTimeout() const;
    const SslConfiguration& getSslConfiguration() const;
    const bool& isTcpNoDelay() const;
    const int& getValueSizeEstimate() const;

  private:
    std::string protocolVersion;
    ConnectionPoolConfiguration connectionPoolConfiguration;
    int connectionTimeout;
    bool forceReturnValue;
    int keySizeEstimate;
    bool pingOnStartup;
    std::vector<ServerConfiguration> servers;
    int socketTimeout;
    SslConfiguration sslConfiguration;
    bool tcpNoDelay;
    int valueSizeEstimate;
};

}} // namespace

#endif /* ISPN_HOTROD_CONFIGURATION_H */
