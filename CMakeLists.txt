cmake_minimum_required(VERSION 3.0.0)
project(infinispan-hotrod-cpp VERSION 0.1.0)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/infinispan/hotrod)
find_package(Protobuf REQUIRED)

# OpenSSL support
if(NOT DEFINED WIN32)
  find_package(OpenSSL)
  include_directories(${OPENSSL_INCLUDE_DIR})
endif(NOT DEFINED WIN32)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_BINARY_DIR}")

set(INCLUDE_FILES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(DLLEXPORT "HR_PROTO_EXPORT=__attribute__ ((visibility (\"default\")))")
set(DLLEXPORT_STATIC "HR_PROTO_EXPORT= ")

function(hr_protobuf_generate_cpp SRCS HDRS DEST_PATH)
  if(NOT ARGN)
    message(SEND_ERROR "Error: PROTOBUF_GENERATE_CPP() called without any proto files")
    return()
  endif()

  if(PROTOBUF_GENERATE_CPP_APPEND_PATH)
    # Create an include path for each file specified
    foreach(FIL ${ARGN})
      get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
      get_filename_component(ABS_PATH ${ABS_FIL} PATH)
      list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)

      if(${_contains_already} EQUAL -1)
        list(APPEND _protobuf_include_path -I ${ABS_PATH})
      endif()
    endforeach()
  else()
    set(_protobuf_include_path -I ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  if(DEFINED PROTOBUF_IMPORT_DIRS)
    foreach(DIR ${PROTOBUF_IMPORT_DIRS})
      get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
      list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)

      if(${_contains_already} EQUAL -1)
        list(APPEND _protobuf_include_path -I ${ABS_PATH})
      endif()
    endforeach()
  endif()

  set(${SRCS})
  set(${HDRS})

  foreach(FIL ${ARGN})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)

    list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${DEST_PATH}/${FIL_WE}.pb.cc")
    list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${DEST_PATH}/${FIL_WE}.pb.h")

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${DEST_PATH}/${FIL_WE}.pb.cc"
      "${CMAKE_CURRENT_BINARY_DIR}/${DEST_PATH}/${FIL_WE}.pb.h"
      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --cpp_out dllexport_decl=HR_PROTO_EXPORT:${CMAKE_CURRENT_BINARY_DIR}/${DEST_PATH} ${_protobuf_include_path} ${ABS_FIL}
      DEPENDS ${ABS_FIL}
      COMMENT "Running C++ protocol buffer compiler on ${FIL}"
      VERBATIM)
  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

hr_protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS infinispan/hotrod
  proto/infinispan/hotrod/protobuf/base_types.proto
  proto/org/infinispan/protostream/message-wrapping.proto
  proto/org/infinispan/query/remote/client/query.proto
)

include_directories(${PROTOBUF_INCLUDE_DIRS})

add_library(hotrod_protobuf STATIC ${PROTO_SRCS})

set_target_properties(hotrod_protobuf PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(hotrod_protobuf PROPERTIES COMPILE_FLAGS "-fPIC ${WARNING_FLAGS}")
target_include_directories(hotrod_protobuf PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
  "${INCLUDE_FILES_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}"
  "${PROTOBUF_INCLUDE_DIR}")

configure_file(src/hotrod/impl/Version.cpp.in ${CMAKE_BINARY_DIR}/Version.cpp @ONLY)

if(WIN32 AND NOT CYGWIN)
  # linking against Windows native libraries, including mingw
  set(HOTROD_WINAPI TRUE)
  set(platform_libs ws2_32 Rpcrt4)
elseif(APPLE)
  # OSX does not need rt
  set(platform_libs pthread sasl2)
else(WIN32 AND NOT CYGWIN)
  set(platform_libs pthread rt sasl2 krb5)
endif(WIN32 AND NOT CYGWIN)

if(HOTROD_WINAPI)
  set(platform_sources src/hotrod/sys/windows/Socket.cpp src/hotrod/sys/windows/Thread.cpp
    src/hotrod/sys/windows/platform.cpp src/hotrod/sys/windows/Inet.cpp src/hotrod/sys/windows/Time.cpp)
else(HOTROD_WINAPI)
  set(platform_sources src/hotrod/sys/posix/Socket.cpp src/hotrod/sys/posix/Thread.cpp
    src/hotrod/sys/posix/platform.cpp src/hotrod/sys/posix/Mutex.cpp src/hotrod/sys/posix/Inet.cpp src/hotrod/sys/posix/Time.cpp)
endif(HOTROD_WINAPI)

if(WIN32)
  set(socket_impl
    src/hotrod/impl/transport/tcp/SChannelTcpTransport.cpp
    src/hotrod/sys/SChannelSocket.cpp
  )
else(WIN32)
  set(socket_impl
    src/hotrod/impl/transport/tcp/SSLTcpTransport.cpp
    src/hotrod/sys/SSLSocket.cpp
  )
endif(WIN32)

set(library_sources
  src/hotrod/api/RemoteCacheManager.cpp
  src/hotrod/api/RemoteCacheBase.cpp
  src/hotrod/api/exceptions.cpp
  src/hotrod/api/RemoteCacheManagerAdmin.cpp
  src/hotrod/impl/configuration/Configuration.cpp
  src/hotrod/impl/configuration/ConnectionPoolConfiguration.cpp
  src/hotrod/impl/configuration/ConfigurationChildBuilder.cpp
  src/hotrod/impl/RemoteCacheManagerImpl.cpp
  src/hotrod/impl/RemoteCacheImpl.cpp
  src/hotrod/impl/Topology.cpp
  src/hotrod/impl/TopologyInfo.cpp
  src/hotrod/impl/hash/MurmurHash3.cpp
  src/hotrod/impl/consistenthash/SegmentConsistentHash.cpp
  src/hotrod/impl/operations/OperationsFactory.cpp
  src/hotrod/impl/operations/QueryOperation.cpp
  src/hotrod/impl/operations/PingOperation.cpp
  src/hotrod/impl/operations/GetOperation.cpp
  src/hotrod/impl/operations/GetAllOperation.cpp
  src/hotrod/impl/operations/PutOperation.cpp
  src/hotrod/impl/operations/PutIfAbsentOperation.cpp
  src/hotrod/impl/operations/ReplaceOperation.cpp
  src/hotrod/impl/operations/RemoveOperation.cpp
  src/hotrod/impl/operations/ContainsKeyOperation.cpp
  src/hotrod/impl/operations/ReplaceIfUnmodifiedOperation.cpp
  src/hotrod/impl/operations/RemoveIfUnmodifiedOperation.cpp
  src/hotrod/impl/operations/GetWithMetadataOperation.cpp
  src/hotrod/impl/operations/GetWithVersionOperation.cpp
  src/hotrod/impl/operations/BulkGetOperation.cpp
  src/hotrod/impl/operations/BulkGetKeysOperation.cpp
  src/hotrod/impl/operations/StatsOperation.cpp
  src/hotrod/impl/operations/ClearOperation.cpp
  src/hotrod/impl/operations/SizeOperation.cpp
  src/hotrod/impl/operations/FaultTolerantPingOperation.cpp
  src/hotrod/impl/operations/ExecuteCmdOperation.cpp
  src/hotrod/impl/protocol/HeaderParams.cpp
  src/hotrod/impl/protocol/Codec20.cpp
  src/hotrod/impl/protocol/Codec21.cpp
  src/hotrod/impl/protocol/Codec22.cpp
  src/hotrod/impl/protocol/Codec26.cpp
  src/hotrod/impl/protocol/Codec28.cpp
  src/hotrod/impl/protocol/CodecFactory.cpp
  src/hotrod/impl/transport/AbstractTransport.cpp
  src/hotrod/impl/transport/tcp/ConnectionPool.cpp
  src/hotrod/impl/transport/tcp/InetSocketAddress.cpp
  src/hotrod/impl/transport/tcp/Socket.cpp
  src/hotrod/impl/transport/tcp/TcpTransport.cpp
  src/hotrod/impl/transport/TransportFactory.cpp
  src/hotrod/impl/transport/tcp/TransportObjectFactory.cpp
  src/hotrod/impl/transport/tcp/RoundRobinBalancingStrategy.cpp
  src/hotrod/sys/Runnable.cpp
  src/hotrod/sys/Log.cpp
  ${socket_impl}
  src/hotrod/impl/event/JBasicEventMarshaller.cpp
  src/hotrod/impl/event/ClientListenerNotifier.cpp
  src/hotrod/impl/operations/AddClientListenerOperation.cpp
  src/hotrod/impl/operations/RemoveClientListenerOperation.cpp
  src/hotrod/impl/operations/AuthOperation.cpp
  src/hotrod/impl/operations/AuthMechListOperation.cpp
  src/hotrod/impl/event/EventDispatcher.cpp
  src/hotrod/impl/operations/CounterOperations.cpp
  src/hotrod/api/RemoteCounterManagerImpl.cpp
  src/hotrod/api/CountersImpl.cpp
  src/hotrod/api/TransactionManager.cpp
  src/hotrod/impl/operations/TransactionOperations.cpp
  ${platform_sources}
  ${CMAKE_BINARY_DIR}/Version.cpp
)

add_library(hotrod SHARED ${library_sources})

set_target_properties(hotrod PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(hotrod PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS} ${STATIC_FLAGS}")
target_link_libraries(hotrod ${platform_libs} ${OPENSSL_LIBRARIES} hotrod_protobuf ${PROTOBUF_LIBRARY})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
