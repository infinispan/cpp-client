#!/bin/bash
unzip zip/$SERVER_ZIP
export INFINISPAN_SERVER=${SERVER_ZIP/.zip/}
cp ./cpp-client/test/data/* $INFINISPAN_SERVER/server/conf
cp -rf $INFINISPAN_SERVER/server $INFINISPAN_SERVER/server1
mkdir build && cd build
cmake -DINSTALL_GTEST=FALSE -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DHR_USE_SYSTEM_PROTOBUF=TRUE -DHOTROD_PREBUILT_LIB_DIR=/home/jboss/inst/usr/lib64 -DNOENABLE_WARNING_ERROR=TRUE -DHOTROD_JBOSS_HOME=/home/jboss/${INFINISPAN_SERVER} /home/jboss/cpp-client/
cmake --build . --config RelWithDebInfo --target build_test
ctest -V
