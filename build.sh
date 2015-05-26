#!/bin/bash

INFINISPAN_VERSION=7.2.1.Final
BUILD_DIR=build

wget -N http://downloads.jboss.org/infinispan/${INFINISPAN_VERSION}/infinispan-server-${INFINISPAN_VERSION}-bin.zip 

rm -rf infinispan-server-${INFINISPAN_VERSION}
unzip infinispan-server-${INFINISPAN_VERSION}-bin.zip
export JBOSS_HOME=`pwd`/infinispan-server-${INFINISPAN_VERSION}

rm -rf ${BUILD_DIR} &&
mkdir ${BUILD_DIR} &&
cd ${BUILD_DIR} &&
cmake .. &&
cmake --build . &&
ctest -V &&
cmake --build . --target memtestv &&
cpack -G RPM
