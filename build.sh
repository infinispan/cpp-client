#!/bin/bash

JDG_VERSION=7.0.0
MILESTONE_VERSION=DR1
BUILD_DIR=build

wget -N http://download.eng.bos.redhat.com/devel/candidates/JDG/JDG-$JDG_VERSION-$MILESTONE_VERSION/jboss-datagrid-$JDG_VERSION.$MILESTONE_VERSION-server.zip

rm -rf jboss-datagrid-${INFINISPAN_VERSION}
unzip jboss-datagrid-$JDG_VERSION.$MILESTONE_VERSION-server.zip
export JBOSS_HOME=`pwd`/infinispan-server-${INFINISPAN_VERSION}
echo $1
if [  $1 == DEBUG ]
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
