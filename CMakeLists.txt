#
# JBoss, Home of Professional Open Source
# Copyright 2010 Red Hat Inc. and/or its affiliates and other
# contributors as indicated by the @author tags. All rights reserved.
# See the copyright.txt in the distribution for a full listing of
# individual contributors.
#
# This is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of
# the License, or (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this software; if not, write to the Free
# Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301 USA, or see the FSF site: http://www.fsf.org.
#

cmake_minimum_required (VERSION 2.6)

project (HotRod C CXX)

#file(READ version.txt HR_VERSION_FILE)
#string(REGEX MATCHALL "[0-9]+" HR_VERSION_LIST "${HR_VERSION_FILE}")

#list(GET HR_VERSION_LIST 0 HR_VERSION_MAJOR)
#list(GET HR_VERSION_LIST 1 HR_VERSION_MINOR)

#set (HR_VERSION "${HR_VERSION_MAJOR}.${HR_VERSION_MINOR}")
#message(STATUS "HR_VERSION: ${HR_VERSION}")

enable_testing()
include (CTest)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src")

if (CMAKE_COMPILER_IS_GNUCXX)
   set (COMPILER_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")
# ZZZ fix swig call for -Werror 
   set (WARNING_FLAGS
        "-pedantic -Wall -Wextra -Wno-shadow -Wpointer-arith -Wcast-qual -Wcast-align -Wno-long-long -Wvolatile-register-var -Winvalid-pch -Wno-system-headers -Woverloaded-virtual -Wshadow -Wl,--no-undefined")
endif (CMAKE_COMPILER_IS_GNUCXX)

if (MSVC)
   set (COMPILER_FLAGS "")
   set (WARNING_FLAGS "")
endif (MSVC)

if (NOT DEFINED COMPILER_FLAGS)
    message(FATAL_ERROR "Compiler flags not set for this build type")
endif (NOT DEFINED COMPILER_FLAGS)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS} ${WARNING_FLAGS}")

if(WIN32 AND NOT CYGWIN)
  # linking against Windows native libraries, including mingw
  set (HOTROD_WINAPI TRUE)
  set (platform_libs ws2_32 Rpcrt4)
endif(WIN32 AND NOT CYGWIN)

# Select driver
if(HOTROD_WINAPI)
  set (platform_sources src/hotrod/sys/windows/Socket.cpp)
else(HOTROD_WINAPI)
  set (platform_sources src/hotrod/sys/posix/Socket.cpp)
endif(HOTROD_WINAPI)



add_library (
  hotrod SHARED
  src/hotrod/api/RemoteCacheManager.cpp
  src/hotrod/api/RemoteCacheBase.cpp
  src/hotrod/api/exceptions.cpp
  src/hotrod/api/Configuration.cpp
  src/hotrod/configuration/ConfigurationBuilder.cpp
  src/hotrod/impl/RemoteCacheManagerImpl.cpp
  src/hotrod/impl/RemoteCacheImpl.cpp
  src/hotrod/impl/operations/OperationsFactory.cpp
  src/hotrod/impl/operations/PingOperation.cpp
  src/hotrod/impl/operations/GetOperation.cpp
  src/hotrod/impl/operations/PutOperation.cpp
  src/hotrod/impl/protocol/HeaderParams.cpp
  src/hotrod/impl/protocol/Codec12.cpp
  src/hotrod/impl/protocol/CodecFactory.cpp
  src/hotrod/impl/transport/AbstractTransport.cpp
  src/hotrod/impl/transport/TransportFactory.cpp
  src/hotrod/impl/transport/tcp/TcpTransport.cpp
  src/hotrod/impl/transport/tcp/TcpTransportFactory.cpp
  src/hotrod/impl/transport/tcp/InputStream.cpp
  ${platform_sources}
  )
target_link_libraries (hotrod ${platform_libs})

add_executable (simple test/Simple.cpp)
target_link_libraries (simple hotrod)
add_test (simple simple)

add_subdirectory(test/swig)
