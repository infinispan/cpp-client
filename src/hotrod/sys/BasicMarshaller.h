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

}}} // namespace

#endif  /* ISPN_HOTROD_BASICMARSHALLER_H */
