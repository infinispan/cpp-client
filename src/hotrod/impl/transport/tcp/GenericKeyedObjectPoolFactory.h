#ifndef ISPN_HOTROD_TRANSPORT_GENERICKEYEDOBJECTPOOLFACTORY_H
#define ISPN_HOTROD_TRANSPORT_GENERICKEYEDOBJECTPOOLFACTORY_H

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
#include "hotrod/impl/transport/tcp/GenericKeyedObjectPool.h"

namespace infinispan {
namespace hotrod {
namespace transport {

template <class K, class V> class GenericKeyedObjectPoolFactory
{
  public:
	GenericKeyedObjectPoolFactory(
      KeyedPoolableObjectFactory<K,V>& factory_,
      int maxActive_,
      WhenExhaustedAction whenExhaustedAction_,
      long maxWait_,
      int maxIdle_,
      int maxTotal_,
      int minIdle_,
      bool testOnBorrow_,
      bool testOnReturn_,
      long timeBetweenEvictionRunsMillis_,
      int numTestsPerEvictionRun_,
      long minEvictableIdleTimeMillis_,
      bool testWhileIdle_,
      bool lifo_)
      : factory(factory_), maxIdle(maxIdle_), maxActive(maxActive_),
        maxTotal(maxTotal_), minIdle(minIdle_), maxWait(maxWait_),
        whenExhaustedAction(whenExhaustedAction_), testOnBorrow(testOnBorrow_),
        testOnReturn(testOnReturn_), testWhileIdle(testWhileIdle_),
        timeBetweenEvictionRunsMillis(timeBetweenEvictionRunsMillis_),
        numTestsPerEvictionRun(numTestsPerEvictionRun_),
        minEvictableIdleTimeMillis(minEvictableIdleTimeMillis_), lifo(lifo_) { }

    int getMaxIdle() { return maxIdle; }
    int getMaxActive() { return maxActive; }
    int getMaxTotal() { return maxTotal; }
    int getMinIdle() { return minIdle; }
    long getMaxWait() { return maxWait; }
    WhenExhaustedAction getWhenExhaustedAction() { return whenExhaustedAction; }
    bool hasTestOnBorrow() { return testOnBorrow; }
    bool hasTestOnReturn() { return testOnReturn; }
    bool hasTestWhileIdle() { return testWhileIdle; }
    int getTimeBetweenEvictionRunsMillis() { return timeBetweenEvictionRunsMillis; }
    int getNumTestsPerEvictionRun() { return numTestsPerEvictionRun; }
    int getMinEvictableIdleTimeMillis() { return minEvictableIdleTimeMillis; }
    bool isLifo() { return lifo; }

    // TODO
    GenericKeyedObjectPool<K,V>* createPool()
    {
        return new GenericKeyedObjectPool<K,V>(
            factory,
            maxActive,
            whenExhaustedAction,
            maxWait,
            maxIdle,
            maxTotal,
            minIdle,
            testOnBorrow,
            testOnReturn,
            timeBetweenEvictionRunsMillis,
            numTestsPerEvictionRun,
            minEvictableIdleTimeMillis,
            testWhileIdle,
            lifo
        );
    }

    ~GenericKeyedObjectPoolFactory() {/*delete &factory;*/} // SEGMENTATION FAULT

  private:
    KeyedPoolableObjectFactory<K,V>& factory;

    int maxIdle;
    int maxActive;
    int maxTotal;
    int minIdle;
    long maxWait;
    WhenExhaustedAction whenExhaustedAction;
    bool testOnBorrow;
    bool testOnReturn;
    bool testWhileIdle;
    long timeBetweenEvictionRunsMillis;
    int numTestsPerEvictionRun;
    long minEvictableIdleTimeMillis;
    bool lifo;
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_GENERICKEYEDOBJECTPOOLFACTORY_H */
