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

#include "hotrod/impl/configuration/Configuration.h"

namespace infinispan {
namespace hotrod {

const char* Configuration::PROTOCOL_VERSION_12 = "1.2";

Configuration::Configuration(std::string protocolVersion,
        const ConnectionPoolConfiguration& cpc,
        int connectionTimeout,
        bool forceReturnValue,
        int keySizeEstimate,
        bool pingOnStartup,
        std::vector<ServerConfiguration,std::allocator<ServerConfiguration> > serversConfiguration,
        int socketTimeout,
        SslConfiguration sslConfiguration,
        bool tcpNoDelay,
        int valueSizeEstimate) : connectionPoolConfiguration(cpc)
{
  Configuration::protocolVersion = Configuration::PROTOCOL_VERSION_12;
  //Configuration::connectionPoolConfiguration = connectionPoolConfiguration;
  Configuration::connectionTimeout = connectionTimeout;
  Configuration::forceReturnValue = forceReturnValue;
  Configuration::keySizeEstimate = keySizeEstimate;
  Configuration::pingOnStartup = pingOnStartup;
  Configuration::servers = serversConfiguration;
  Configuration::socketTimeout = socketTimeout;
  Configuration::sslConfiguration = sslConfiguration;
  Configuration::tcpNoDelay = tcpNoDelay;
  Configuration::valueSizeEstimate = valueSizeEstimate;
}

const std::string& Configuration::getProtocolVersion() const
{
  return Configuration::protocolVersion;
}

const ConnectionPoolConfiguration& Configuration::getConnectionPoolConfiguration() const
{
  return Configuration::connectionPoolConfiguration;
}

const int& Configuration::getConnectionTimeout() const
{
  return Configuration::connectionTimeout;
}

const bool& Configuration::isForceReturnValue() const
{
  return Configuration::forceReturnValue;
}

const int& Configuration::getKeySizeEstimate() const
{
  return Configuration::keySizeEstimate;
}

const bool& Configuration::isPingOnStartup() const
{
  return Configuration::pingOnStartup;
}

const std::vector<ServerConfiguration>& Configuration::getServersConfiguration() const
{
  return Configuration::servers;
}

const int& Configuration::getSocketTimeout() const
{
  return Configuration::socketTimeout;
}

const SslConfiguration& Configuration::getSslConfiguration() const
{
  return Configuration::sslConfiguration;
}

const bool& Configuration::isTcpNoDelay() const
{
  return Configuration::tcpNoDelay;
}

const int& Configuration::getValueSizeEstimate() const
{
  return Configuration::valueSizeEstimate;
}

}} /* namespace */
