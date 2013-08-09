#ifndef ISPN_HOTROD_BASICMARSHALLER_H
#define ISPN_HOTROD_BASICMARSHALLER_H

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
#include <iostream>
#include "infinispan/hotrod/Marshaller.h"

namespace infinispan {
namespace hotrod {
namespace sys {

/*
 * A Marshaller for a few simple types.
 */


template <class T> class BasicMarshaller : public infinispan::hotrod::Marshaller<T>
{
};

class BasicMarshallerHelper {
  public:
    static void noRelease(ScopedBuffer*) { /* nothing allocated, nothing to release */ }
};


// Specialization for std::string:

template <>
class BasicMarshaller<std::string> : public infinispan::hotrod::Marshaller<std::string> {
  public:
    void marshall(const std::string& s, ScopedBuffer& b) {
        b.set(const_cast<char *> (s.data()), s.size(), &BasicMarshallerHelper::noRelease);
    }
    std::string* unmarshall(const ScopedBuffer& b) {
        std::string* s = new std::string(b.getBytes(), b.getLength());
        return s;
    }
};

template <>
class BasicMarshaller<int> : public infinispan::hotrod::Marshaller<int> {
  public:
    void marshall(const int& s, ScopedBuffer& b) {
    	  char *buf = new char[4];
    	  for (int i = 0 ; i < 4 ; i++) {
    	    buf[3-i] = (char) ((s) >> (8*i));
    	  }
        b.set(buf, 4, &BasicMarshallerHelper::noRelease);
    }
    int* unmarshall(const ScopedBuffer& b) {
    	int result = 0;
    	  for (int i = 0; i < 4 ; i++) {
    	    result <<= 4;
    	    result ^= (int) *(b.getBytes()+i) & 0xFF;
    	  }
        int* s = new int(result);
        return s;
    }
};

}}} // namespace

#endif  /* ISPN_HOTROD_BASICMARSHALLER_H */
