#ifndef ISPN_HOTROD_TRANSPORT_GENERICKEYEDOBJECTPOOL_H
#define ISPN_HOTROD_TRANSPORT_GENERICKEYEDOBJECTPOOL_H

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

namespace infinispan {
namespace hotrod {
namespace transport {

template <class K, class V> class GenericKeyedObjectPoolFactory;

enum WhenExhaustedAction {
  WHEN_EXHAUSTED_GROW,
  WHEN_EXHAUSTED_FAIL,
  WHEN_EXHAUSTED_BLOCK
};

template <class K, class V> class GenericKeyedObjectPool
{
  public:
    GenericKeyedObjectPool(
      KeyedPoolableObjectFactory<K,V>& factory_,
      int maxActive_,
      WhenExhaustedAction whenExhaustedAction_,
      long maxWait_,
      int maxIdle_,
      int maxTotal_,
      int minIdle_,
      bool testOnBorrow_,
      bool testOnReturn_,
      long timeBetweenEvictionRuns_,
      int numTestsPerEvictionRun_,
      long minEvictableIdleTime_,
      bool testWhileIdle_,
      bool lifo_)
      : factory(factory_), maxIdle(maxIdle_), maxActive(maxActive_),
        maxTotal(maxTotal_), minIdle(minIdle_), maxWait(maxWait_),
        whenExhaustedAction(whenExhaustedAction_), testOnBorrow(testOnBorrow_),
        testOnReturn(testOnReturn_), testWhileIdle(testWhileIdle_),
        timeBetweenEvictionRunsMillis(timeBetweenEvictionRuns_),
        numTestsPerEvictionRun(numTestsPerEvictionRun_),
        minEvictableIdleTimeMillis(minEvictableIdleTime_), lifo(lifo_) { }

    // TODO: ALL IMPLEMENTATION

    int getNumActive() { return 0; }
    int getNumActive(const K& key) { return 0; }
    int getNumIdle() { return 0; }
    int getNumIdle(const K& key) { return 0; }

    void returnObject(const K& key, V& val) { factory.destroyObject(key, val); }
    V& borrowObject(const K& key) { return factory.makeObject(key); }
    void invalidateObject(const K& /*key*/, V* /*val*/) { }
    void clear() { }
    void clear(const K& key) { }

    int getMaxActive() { return maxActive; }
    void preparePool(const InetSocketAddress& /*key*/) { }
    void close() { }

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

#endif  /* ISPN_HOTROD_TRANSPORT_GENERICKEYEDOBJECTPOOL_H */
