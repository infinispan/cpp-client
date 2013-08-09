#ifndef ISPN_HOTROD_TRANSPORT_PROPSKEYEDOBJECTPOOL_H
#define ISPN_HOTROD_TRANSPORT_PROPSKEYEDOBJECTPOOL_H

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

#include "hotrod/impl/transport/tcp/KeyedPoolableObjectFactory.h"
#include "hotrod/impl/transport/tcp/GenericKeyedObjectPoolFactory.h"
#include "hotrod/impl/configuration/ConnectionPoolConfiguration.h"

namespace infinispan {
namespace hotrod {
namespace transport {

template <class K, class V> class PropsKeyedObjectPoolFactory :
  public GenericKeyedObjectPoolFactory<K,V>
{
  public:
	PropsKeyedObjectPoolFactory(
      KeyedPoolableObjectFactory<K,V>& factory_,
      const ConnectionPoolConfiguration& configuration)
      : GenericKeyedObjectPoolFactory<K,V>(
        factory_,
        configuration.getMaxActive(),
        mapExhaustedAction(configuration.getExhaustedAction()),
        configuration.getMaxWait(),
        configuration.getMaxIdle(),
        configuration.getMaxTotal(),
        configuration.getMinIdle(),
        configuration.isTestOnBorrow(),
        configuration.isTestOnReturn(),
        configuration.isTestWhileIdle(),
        configuration.getTimeBetweenEvictionRuns(),
        configuration.getNumTestsPerEvictionRun(),
        configuration.getMinEvictableIdleTime(),
        configuration.isLifo())
    { }

  private:
	WhenExhaustedAction mapExhaustedAction(ExhaustedAction exhaustedAction) {
      switch(exhaustedAction) {
      case CREATE_NEW:
    	  return WHEN_EXHAUSTED_GROW;
      case EXCEPTION:
    	  return WHEN_EXHAUSTED_FAIL;
      case WAIT:
      default:
    	  return WHEN_EXHAUSTED_BLOCK;
      }
	}
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_PROPSKEYEDOBJECTPOOL_H */
