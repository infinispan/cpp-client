#ifndef ISPN_HOTROD_EXCEPTIONS_H
#define ISPN_HOTROD_EXCEPTIONS_H

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

#include "infinispan/hotrod/ImportExport.h"
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"
#include <exception>
#include <string>

namespace infinispan {
namespace hotrod {

class HR_EXTERN Exception : public std::exception
{
  public:
    explicit Exception(const std::string& message=std::string()) throw();
    virtual ~Exception() throw();
    virtual const char* what() const throw();

  private:
    const std::string message;
};

// Matching the names of the Java exceptions where possible, adding if
// necessary.

struct HR_EXTERN HotRodClientException : public Exception
{
    HotRodClientException(const std::string&);
    //virtual ~HotRodClientException() throw();
};

struct HR_EXTERN TransportException : public HotRodClientException
{
    transport::InetSocketAddress serverAddress;
    TransportException(const std::string& host, int port,
    		const std::string&);
    ~TransportException() throw();

    const transport::InetSocketAddress& getServerAddress() const;
};

struct HR_EXTERN InvalidResponseException : public HotRodClientException
{
	InvalidResponseException(const std::string&);
};

struct HR_EXTERN RemoteNodeSuspectException : public HotRodClientException
{
	RemoteNodeSuspectException(const std::string&);
};

struct HR_EXTERN InternalException : public HotRodClientException
{
	InternalException(const std::string&);
};

}} // namespace

#endif  /* ISPN_HOTROD_EXCEPTIONS_H */
