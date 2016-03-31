#!/bin/bash

if [ "$INFINISPAN_VERSION" == "" ]
  then
  echo Please define INFINISPAN_VERSION variable
  exit 1
fi

BUILD_DIR=build

wget -N http://downloads.jboss.org/infinispan/${INFINISPAN_VERSION}/infinispan-server-${INFINISPAN_VERSION}-bin.zip 

rm -rf infinispan-server-${INFINISPAN_VERSION}
unzip infinispan-server-${INFINISPAN_VERSION}-bin.zip
export JBOSS_HOME=`pwd`/infinispan-server-${INFINISPAN_VERSION}
if [  "$1" == "DEBUG" ]
then
  rm -rf ${BUILD_DIR} &&
  mkdir ${BUILD_DIR} &&
  cd ${BUILD_DIR} &&
  cmake -DCMAKE_BUILD_TYPE=Debug .. &&
  cmake --build .
else
  rm -rf ${BUILD_DIR} &&
  mkdir ${BUILD_DIR} &&
  cd ${BUILD_DIR} &&
  cmake .. &&
  cmake --build . &&
  ctest -V &&
  cpack -G RPM
fi
