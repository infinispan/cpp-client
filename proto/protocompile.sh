#!/bin/bash
mkdir out
protoc -I=. --cpp_out=out  org/infinispan/query/remote/client/query.proto org/infinispan/protostream/message-wrapping.proto infinispan/hotrod/protobuf/base_types.proto
mv `find out -name "*.cc"` ../src/hotrod/api
mv out/org  ../include
mv out/infinispan/hotrod/protobuf  ../include/infinispan/hotrod
rm -rf out
