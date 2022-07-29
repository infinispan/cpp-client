#!/bin/bash

if [ "$INFINISPAN_VERSION" == "" ]
  then
  echo Please define INFINISPAN_VERSION variable
  exit 1
fi

BUILD_DIR=build
wget --progress=dot:giga -N http://downloads.jboss.org/infinispan/${INFINISPAN_VERSION}/infinispan-server-${INFINISPAN_VERSION}.zip

rm -rf infinispan-server-${INFINISPAN_VERSION}
unzip -q infinispan-server-${INFINISPAN_VERSION}.zip
export JBOSS_HOME=`pwd`/infinispan-server-${INFINISPAN_VERSION}
cp test/data/* $JBOSS_HOME/server/conf
# Copy dir for near cache and cluster test
cp -rf $JBOSS_HOME/server $JBOSS_HOME/server1

if [  "${CLIENT_VERSION}" != "" ]
then
  V1=${CLIENT_VERSION/*\//}
  VER_ARR=(${V1//./ })
fi

if [  "$VER_ARR[0]}" != "" ]
then
  CMAKE_EXTRAS="${CMAKE_EXTRAS} -DCPACK_PACKAGE_VERSION_MAJOR=${VER_ARR[0]}"
fi

if [  "$VER_ARR[1]}" != "" ]
then
  CMAKE_EXTRAS="${CMAKE_EXTRAS} -DCPACK_PACKAGE_VERSION_MINOR=${VER_ARR[1]}"
fi

if [  "${VER_ARR[2]}" != "" ]
then
  if [  "${VER_ARR[3]}" != "" ]
  then
    VER_ARR[2]=${VER_ARR[2]}.${VER_ARR[3]}
  fi
  CMAKE_EXTRAS="${CMAKE_EXTRAS} -DCPACK_PACKAGE_VERSION_PATCH=${VER_ARR[2]}"
fi

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

if [  "$PLATFORM_TAG" != "" ]
then
  CMAKE_EXTRAS="${CMAKE_EXTRAS} -DPLATFORM_TAG=${PLATFORM_TAG}"
fi

if [  "$1" == "DEBUG" ]
then
  rm -rf ${BUILD_DIR} &&
  mkdir ${BUILD_DIR} &&
  cd ${BUILD_DIR} &&
  echo cmake -DCMAKE_BUILD_TYPE=Debug ${CMAKE_EXTRAS}.. &&
  CXXFLAGS="-Wno-error=maybe-uninitialized" cmake -DCMAKE_BUILD_TYPE=Debug ${CMAKE_EXTRAS}.. &&
  cmake --build .
else
  rm -rf ${BUILD_DIR} &&
  mkdir ${BUILD_DIR} &&
  cd ${BUILD_DIR} &&
  echo CXXFLAGS="-Wno-error=maybe-uninitialized" cmake ${CMAKE_EXTRAS} .. &&
  CXXFLAGS="-Wno-error=maybe-uninitialized" cmake ${CMAKE_EXTRAS} .. &&
  cmake --build . &&
  if [ "x$SKIP_TESTS" == "x" ]; then ctest -V ; fi &&
  cpack -G RPM &&
  cpack -C RelWithDebInfo --config CPackSourceConfig.cmake -G ZIP
fi
