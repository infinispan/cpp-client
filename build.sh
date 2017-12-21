#!/bin/bash

if [ "$INFINISPAN_VERSION" == "" ]
  then
  echo Please define INFINISPAN_VERSION variable
  exit 1
fi

BUILD_DIR=build

wget --progress=dot:giga -N http://downloads.jboss.org/infinispan/${INFINISPAN_VERSION}/infinispan-server-${INFINISPAN_VERSION}-bin.zip 

rm -rf infinispan-server-${INFINISPAN_VERSION}
unzip -q infinispan-server-${INFINISPAN_VERSION}-bin.zip
export JBOSS_HOME=`pwd`/infinispan-server-${INFINISPAN_VERSION}
cp test/data/* $JBOSS_HOME/standalone/configuration

if [  "${PROTOBUF_LIBRARY}" != "" ]
then
  CMAKE_EXTRAS="${CMAKE_EXTRAS} -DPROTOBUF_LIBRARY=${PROTOBUF_LIBRARY}"
fi

if [  "${PROTOBUF_PROTOC_LIBRARY}" != "" ]
then
  CMAKE_EXTRAS="${CMAKE_EXTRAS} -DPROTOBUF_PROTOC_LIBRARY=${PROTOBUF_PROTOC_LIBRARY}"
fi

if [  "$PROTOBUF_PROTOC_EXECUTABLE" != "" ]
then
  CMAKE_EXTRAS="${CMAKE_EXTRAS} -DPROTOBUF_PROTOC_EXECUTABLE=${PROTOBUF_PROTOC_EXECUTABLE}"
fi

if [  "$PROTOBUF_INCLUDE_DIR" != "" ]
then
  CMAKE_EXTRAS="${CMAKE_EXTRAS} -DPROTOBUF_INCLUDE_DIR=${PROTOBUF_INCLUDE_DIR}"
fi

if [  "$1" == "DEBUG" ]
then
  rm -rf ${BUILD_DIR} &&
  mkdir ${BUILD_DIR} &&
  cd ${BUILD_DIR} &&
  echo cmake -DCMAKE_BUILD_TYPE=Debug ${CMAKE_EXTRAS}.. &&
  cmake -DCMAKE_BUILD_TYPE=Debug ${CMAKE_EXTRAS}.. &&
  cmake --build .
else
  rm -rf ${BUILD_DIR} &&
  mkdir ${BUILD_DIR} &&
  cd ${BUILD_DIR} &&
  echo cmake ${CMAKE_EXTRAS} .. &&
  cmake ${CMAKE_EXTRAS} .. &&
  cmake --build . &&
  ctest -V &&
  cpack -G RPM &&
  cpack -C RelWithDebInfo --config CPackSourceConfig.cmake -G ZIP
fi
