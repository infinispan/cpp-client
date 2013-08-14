#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#

# FindJava and FindJNI are primitive in cmake 2.6, mimic or go 2.8?
# already done: find_package(Java)
if (NOT JAVA_COMPILE)
    message(FATAL_ERROR "Java javac compiler not found")
endif (NOT JAVA_COMPILE)
message(STATUS "got runtime ${JAVA_RUNTIME}")

find_package(JNI)
if (NOT JNI_LIBRARIES)
    message(FATAL_ERROR "Java JNI support not found")
endif (NOT JNI_LIBRARIES)

find_package(SWIG REQUIRED)
if (NOT SWIG_FOUND)
    message(FATAL_ERROR "SWIG not found")
endif (NOT SWIG_FOUND)
include(UseSWIG)

SET(CMAKE_SWIG_FLAGS -package org.infinispan.client.hotrod.jni)
SET(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/src/org/infinispan/client/hotrod/jni)

message(STATUS "Using JNI libraries: ${JNI_LIBRARIES}" )

SET_SOURCE_FILES_PROPERTIES(java.i PROPERTIES CPLUSPLUS ON)

#add_library (
#  /src/hotrod/api/Configuration.cpp
#)

swig_add_module(hotrod-swig java java.i)
include_directories(${JNI_INCLUDE_DIRS})
swig_link_libraries(hotrod-swig hotrod)

set_target_properties(hotrod-swig
    PROPERTIES
    OUTPUT_NAME "hotrod-jni"
    PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX}")


add_custom_command(OUTPUT ${CMAKE_SWIG_OUTDIR}
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SWIG_OUTDIR}
                   COMMENT "Creating target directory for hotrod-jni sources")
add_custom_target(hotrod-jni-swig-out-dir ALL DEPENDS ${CMAKE_SWIG_OUTDIR})
add_dependencies(hotrod-swig hotrod-jni-swig-out-dir)


