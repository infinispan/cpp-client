#ifndef ISPN_HOTROD_SCOPEDBUFFER_H
#define ISPN_HOTROD_SCOPEDBUFFER_H

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

#include "infinispan/hotrod/exceptions.h"

namespace infinispan {
namespace hotrod {

/*
 * A semi-smart ptr object to hold intermediate marshalled data in the
 * lifetime of a block.  For default case, the bytes are "owned" by
 * the scope and freed when the dtor is called on block exit.  Not for
 * general use.  Intended for short lived serialized bytes passed
 * between a marshaller and Hot Rod.  TODO: better name.
 */

class ScopedBuffer
{
  public:
    typedef void (*ReleaseFunc)(ScopedBuffer *);
    ScopedBuffer() : bytes(0), len(0), release(0) {}
    ~ScopedBuffer() {
        if (release)
            (*release)(this);
        else
            delete[] bytes;
    }
    void set(char *b, size_t l, ReleaseFunc m = 0) {
        if (bytes || len || release) throw HotRodClientException("ScopedBuffer reuse");
        bytes = b; len = l; release = m;
    }
    char* getBytes() const { return bytes; }
    size_t getLength() const { return len; }
    ReleaseFunc getRelease() const { return release; }
    
  private:
    ScopedBuffer(const ScopedBuffer &);
    ScopedBuffer& operator=(ScopedBuffer const &);

    char* bytes;
    size_t len;
    ReleaseFunc release;
};

}} // namespace

#endif  /* ISPN_HOTROD_SCOPEDBUFFER_H */
