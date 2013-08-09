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
#include "hotrod/impl/transport/Transport.h"

#include <iostream>
#include <cstring>
#include <istream>
#include <sstream>


namespace infinispan {
namespace hotrod {

namespace {

static int pinged = 0;

    char* check_header(char *resp, uint8_t expected_op, uint8_t& status) {
    if ((unsigned char)resp[0] != 0xA1)
        throw HotRodClientException("bad magic");
    if (resp[1] != 0)
        throw HotRodClientException("message id parsing missing");
    if (resp[2] != expected_op)
        throw HotRodClientException("wrong message response");
    status = (uint8_t) resp[3];
    if ((status != 0) && (status != 2))
        throw HotRodClientException("straw man falls down");
    if (resp[4] != 0)
        throw HotRodClientException("straw man falls flat");
    return resp+5;
}

int32_t readUnsignedInt(std::stringstream& in) {
    char b;
    in.get(b);
    int32_t i = b & 0x7F;
    for (int shift = 7; (b & 0x80) != 0; shift += 7) {
        in.get(b);
        i |= (b & 0x7FL) << shift;
    }
    return i;
}

int64_t readUnsignedLong(std::stringstream& in) {
    char b;
    in.get(b);
    int64_t i = b & 0x7F;
    for (int shift = 7; (b & 0x80) != 0; shift += 7) {
        in.get(b);
        i |= (b & 0x7FL) << shift;
    }
    return i;
}

void writeUnsignedInt(std::stringstream& out, int32_t i) {
    while ((i & ~0x7F) != 0) {
        char b = (i & 0x7f) | 0x80;
        out.put(b);
        i >>= 7;
    }
    out.put((char) i);
}

void writeUnsignedLong(std::stringstream& out, int64_t i) {
    while ((i & ~0x7F) != 0) {
        char b = (i & 0x7f) | 0x80;
        out.put(b);
        i >>= 7;
    }
    out.put((char) i);
}


int32_t readUnsignedInt(char*& input) {
    char b = *input++;
    int32_t i = b & 0x7F;
    for (int shift = 7; (b & 0x80) != 0; shift += 7) {
        b = *input++;
        i |= (b & 0x7FL) << shift;
    }
    return i;
}

int64_t readUnsignedLong(char*& input) {
    char b = *input++;
    int64_t i = b & 0x7F;
    for (int shift = 7; (b & 0x80) != 0; shift += 7) {
        b = *input++;
        i |= (b & 0x7FL) << shift;
    }
    return i;
}

void writeUnsignedInt(char*& output, int32_t i) {
    while ((i & ~0x7F) != 0) {
        *output++ = (char) ((i & 0x7f) | 0x80);
        i >>= 7;
    }
    *output++ = (char) i;
}

void writeUnsignedLong(char*& output, int64_t i) {
    while ((i & ~0x7F) != 0) {
        *output++ = (char) ((i & 0x7f) | 0x80);
        i >>= 7;
    }
    *output++ = (char) i;
}

} /* namespace */


Transport::Transport() : socket(sys::Socket::create()) {socket->connect();}

Transport* Transport::testTransport = 0;
Transport& Transport::getTestTransport() {
    if (!testTransport)
        testTransport = new Transport();
    return *testTransport;
}


void Transport::strawPing() {
    std::stringstream ss;

    ss.put((char) 0xA0);
    writeUnsignedLong(ss, 0);
    ss.put((char)12);
    ss.put((char) 0x17);
    writeUnsignedInt(ss, 0);
    writeUnsignedInt(ss, 0);
    ss.put((char) 0x1);
    writeUnsignedInt(ss, 0);
    ss.put((char) 0);

    std::string outstr(ss.str());
    socket->write(outstr.data(), outstr.size());
    char buf[1024];
    socket->read(buf, 1024);
    uint8_t status;
    check_header(buf, 0x18, status);
}


hrbytes Transport::strawGet(hrbytes& k) {
    std::stringstream ss;
    // header
    ss.put((char) 0xA0);
    writeUnsignedLong(ss, 0);
    ss.put((char)12);
    ss.put((char) 0x03); // get
    writeUnsignedInt(ss, 0);
    writeUnsignedInt(ss, 0);
    ss.put((char) 0x1);
    writeUnsignedInt(ss, 0);
    ss.put((char) 0);

    //get args
    writeUnsignedInt(ss, k.length());
    ss.write(k.bytes(), k.length());
    
    std::string buf2(ss.str());
    socket->write(buf2.data(), buf2.size());

    char buf[1024];
    unsigned int bufsize = sizeof(buf);
    size_t n = socket->read(buf, bufsize);
    if (n < 5)
        throw HotRodClientException("read error");  // header minimum
    if (n >= 1024) {
        while (socket->read(buf, bufsize) == bufsize)
            ;                   // drain
        throw HotRodClientException("too big for now");
        // TODO: move buffer contents to return value and read rest
        // into it from socket directly
    }
        
    uint8_t status;
    char *resp = check_header(buf, 0x4, status);
    hrbytes rv;
    if (status == 2) // key not found
	return rv;
    uint64_t size = readUnsignedLong(resp);
    rv.reserve(size);
    memcpy(rv.bytes(), resp, size);
    return rv;
}

void Transport::strawPut(hrbytes& k, hrbytes& v) {
    if (!pinged) {
        pinged = 1;
        strawPing();
    }

    std::stringstream ss;
    // header
    ss.put((char) 0xA0);
    writeUnsignedLong(ss, 0);
    ss.put((char)12);
    ss.put((char) 0x01); // put
    writeUnsignedInt(ss, 0);
    writeUnsignedInt(ss, 0);
    ss.put((char) 0x1);
    writeUnsignedInt(ss, 0);
    ss.put((char) 0);

    //put args
    writeUnsignedInt(ss, k.length());
    ss.write(k.bytes(), k.length());
    writeUnsignedInt(ss, 0);
    writeUnsignedInt(ss, 0);
    writeUnsignedInt(ss, v.length());
    ss.write(v.bytes(), v.length());
    
    std::string buf2(ss.str());
    socket->write(buf2.data(), buf2.size());

    char buf[1024];
    socket->read(buf, 1024);
    uint8_t status;
    check_header(buf, 0x2, status);
}


}} /* namespace */
