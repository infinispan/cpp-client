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


#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/ScopedBuffer.h"

#include <iostream>
#include <memory>

using namespace infinispan::hotrod;

int main(int, char**) {
    RemoteCacheManager cacheManager;
    RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>();

    std::string k1("key13");
    std::string k2("key14");
    std::string v1("boron");
    std::string v2("chlorine");

    cache.put(k1, v1);
    std::auto_ptr<std::string> rv(cache.get(k1));
    if (rv->compare(v1)) {
	std::cerr << "get/put fail for " << k1 << " got " << *rv << " expected " << v1 << std::endl;
	return 1;
    }

    cache.put(k2, v2);
    std::auto_ptr<std::string> rv2(cache.get(k2));
    if (rv2->compare(v2)) {
	std::cerr << "get/put fail for " << k2 << " got " << *rv2 << " expected " << v2 << std::endl;
	return 1;
    }

    std::auto_ptr<std::string> badValue(cache.get(std::string("no such key in the cache")));
    if (badValue.get()) {
	std::cout << "non-existent key failure, got " << *badValue << std::endl;
	return -1;
    }

    std::cout << "Simple tests pass" << std::endl;

    return 0;
}
