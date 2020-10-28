cmake_minimum_required (VERSION 2.6)

project (infinispan-hotrod-cpp C CXX)
IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.1)
cmake_policy(SET CMP0054 OLD)
ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.1)


if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # require at least gcc 4.8
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC version must be at least 4.8!")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # require at least clang 3.2
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.2)
        message(FATAL_ERROR "Clang version must be at least 3.2!")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # require at least MS Visual Studio  2012
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 17.0)
        message(FATAL_ERROR "Microsoft Visual Studio version must be at least 2012!")
    endif()
else()
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with Clang, MSVC and GCC.")
endif()

if (NOT DEFINED CPACK_PACKAGE_VERSION_MAJOR OR "${CPACK_PACKAGE_VERSION_MAJOR}" STREQUAL "")
  set (CPACK_PACKAGE_VERSION_MAJOR "8")
endif (NOT DEFINED CPACK_PACKAGE_VERSION_MAJOR OR "${CPACK_PACKAGE_VERSION_MAJOR}" STREQUAL "")

if (NOT DEFINED CPACK_PACKAGE_VERSION_MINOR OR "${CPACK_PACKAGE_VERSION_MINOR}" STREQUAL "")
  set (CPACK_PACKAGE_VERSION_MINOR "1")
endif (NOT DEFINED CPACK_PACKAGE_VERSION_MINOR OR "${CPACK_PACKAGE_VERSION_MINOR}" STREQUAL "")

if (NOT DEFINED CPACK_PACKAGE_VERSION_PATCH OR "${CPACK_PACKAGE_VERSION_PATCH}" STREQUAL "")
  set (CPACK_PACKAGE_VERSION_PATCH "0.SNAPSHOT")
endif (NOT DEFINED CPACK_PACKAGE_VERSION_PATCH OR "${CPACK_PACKAGE_VERSION_PATCH}" STREQUAL "")

set (HOTROD_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set (PROTOCOL_VERSION "HotRod C++ client, protocol version: 2.0-topology_change-event")
message("HOTROD_VERSION is ${HOTROD_VERSION}")

if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  message (FATAL_ERROR "You are calling cmake from the source directory. Please create a separate build directory and call cmake from there. See README.md for details.")
endif ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_BINARY_DIR}")

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and unpack googletest at configure time
configure_file(gtest-CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(WARNING "Build step for googletest failed: ${result}")
  message(WARNING "If you don't need the xunit test suite, this should be ok.")
else()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
  if(result)
    message(WARNING "Build step for googletest failed: ${result}")
    message(WARNING "If you don't need the xunit test suite, this should be ok.")
  else()
    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                     ${CMAKE_BINARY_DIR}/googletest-build
                     EXCLUDE_FROM_ALL)

    # The gtest/gtest_main targets carry header search path
    # dependencies automatically when using CMake 2.8.11 or
    # later. Otherwise we have to add them here ourselves.
    if (CMAKE_VERSION VERSION_LESS 2.8.11)
      include_directories("${gtest_SOURCE_DIR}/include")
    endif()
  endif()
endif()

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/infinispan/hotrod)

find_package(Protobuf REQUIRED)

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
      COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --cpp_out dllexport_decl=HR_PROTO_EXPORT:${CMAKE_CURRENT_BINARY_DIR}/${DEST_PATH} ${_protobuf_include_path} ${ABS_FIL}
      DEPENDS ${ABS_FIL}
      COMMENT "Running C++ protocol buffer compiler on ${FIL}"
      VERBATIM )

  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

include_directories(${PROTOBUF_INCLUDE_DIRS})



if(WIN32 AND NOT CYGWIN)
  # linking against Windows native libraries, including mingw
  set (HOTROD_WINAPI TRUE)
  set (platform_libs ws2_32 Rpcrt4)
elseif (APPLE)
  # OSX does not need rt
  set (platform_libs pthread sasl2)
else (WIN32 AND NOT CYGWIN)
  set (platform_libs pthread rt sasl2 krb5)
endif(WIN32 AND NOT CYGWIN)


if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(DLLEXPORT "HR_PROTO_EXPORT=__attribute__ ((visibility (\"default\")))")
  set(DLLEXPORT_STATIC "HR_PROTO_EXPORT= ")
  if (ENABLE_WARNING_ERROR)
    set (WERROR "-Werror")
  endif (ENABLE_WARNING_ERROR)

  if (ENABLE_INTERNAL_TESTS OR ENABLE_ADDITIONAL_TESTS)
      set (COMPILER_FLAGS "-std=c++11")
  else (ENABLE_INTERNAL_TESTS OR ENABLE_ADDITIONAL_TESTS)
      set (COMPILER_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden -std=c++11")
  endif (ENABLE_INTERNAL_TESTS OR ENABLE_ADDITIONAL_TESTS)
  if (DEFINED CODE_COVERAGE)
        set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)
        include(CodeCoverage)
        setup_target_for_coverage_gcovr_html(NAME hotrod_coverage_html
                                       EXECUTABLE ctest
                                       DEPENDENCIES build_test)
        setup_target_for_coverage_gcovr_xml(NAME hotrod_coverage_xml
                                       EXECUTABLE ctest
                                       DEPENDENCIES build_test)
        set (COMPILER_FLAGS "${COMPILER_FLAGS} ${CMAKE_CXX_FLAGS_COVERAGE}")
  endif (DEFINED CODE_COVERAGE)
  if (CMAKE_COMPILER_IS_GNUCXX)
  set (WARNING_FLAGS
        "${WERROR} -pedantic -Wall -Wextra -Wno-shadow -Wpointer-arith -Wcast-qual -Wcast-align -Wno-long-long -Wvolatile-register-var -Winvalid-pch -Wno-system-headers -Woverloaded-virtual -Wno-variadic-macros -Wno-error=shadow")
  set (WARNING_FLAGS_NO_PEDANTIC
        "${WERROR} -Wall -Wextra -Wno-shadow -Wpointer-arith -Wcast-qual -Wcast-align -Wno-long-long -Wvolatile-register-var -Winvalid-pch -Wno-system-headers -Woverloaded-virtual -Wno-variadic-macros -Wno-error=shadow")
set (NO_UNUSED_FLAGS "-Wno-error=unused-parameter -Wno-unused-parameter")
  endif (CMAKE_COMPILER_IS_GNUCXX)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set (WARNING_FLAGS_NO_PEDANTIC
        "${WERROR} -pedantic -Wall -Wextra -Wno-shadow -Wpointer-arith -Wcast-qual -Wcast-align -Wno-long-long -Wvolatile-register-var -Winvalid-pch -Wno-system-headers -Woverloaded-virtual -Wno-variadic-macros -Wno-error-unused-parameter -Wno-error-cast-align -Wno-error-string-plus-int")
  endif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  if (ENABLE_UNDEFINED_ERROR)
    set (CATCH_UNDEFINED "-Wl,--no-undefined")
  endif (ENABLE_UNDEFINED_ERROR)

  set_source_files_properties(message-wrapping.pb.cc PROPERTIES COMPILE_FLAGS -Wno-error=shadow)
  set_source_files_properties(query.pb.cc PROPERTIES COMPILE_FLAGS -Wno-error=shadow)
  set_source_files_properties(base_types.pb.cc PROPERTIES COMPILE_FLAGS -Wno-error=shadow)

endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")

if (MSVC)
   set(DLLEXPORT "HR_PROTO_EXPORT=__declspec(dllexport)")
   set(DLLIMPORT "HR_PROTO_EXPORT=__declspec(dllimport)")
   set(DLLEXPORT_STATIC "HR_PROTO_EXPORT= ")
   set (COMPILER_FLAGS "/DNOMINMAX /EHsc")
   set (WARNING_FLAGS "/wd4275 /wd4251")
endif (MSVC)

set (STATIC_FLAGS "-DHOTROD_DECLARE_STATIC")

# MacOS has a bunch of differences in build tools and process and so we have to turn some things
# off there by default (for GCC; LLVM will need flags re-evaluated)
if (APPLE)
  set (NOENABLE_UNDEFINED_ERROR ON)
endif (APPLE)

if(WIN32 AND NOT CYGWIN)
  # Encourage use of valgrind unless unavailable on the platform
  set (NOENABLE_VALGRIND ON)
endif(WIN32 AND NOT CYGWIN)

set (NOENABLE_ADDITIONAL_TESTS ON)

# Add options here called <whatever> they will turn into "ENABLE_<whatever" and can be
# overridden on a platform specific basis above by NOENABLE_<whatever>
set (OPTIONS UNDEFINED_ERROR SWIG_TESTING INTERNAL_TESTING VALGRIND ADDITIONAL_TESTS)

foreach (OPTION ${OPTIONS})
  if (NOT "NOENABLE_${OPTION}")
    set ("DEFAULT_${OPTION}" ON)
  endif (NOT "NOENABLE_${OPTION}")
endforeach (OPTION)

# And add the option here too with help text
option(ENABLE_WARNING_ERROR "Consider compiler warnings to be errors" ${DEFAULT_WARNING_ERROR})
option(ENABLE_UNDEFINED_ERROR "Check for unresolved library symbols" ${DEFAULT_UNDEFINED_ERROR})
option(ENABLE_INTERNAL_TESTING "Compile the library with internal tests (unit tests)" ${DEFAULT_INTERNAL_TESTING})
option(ENABLE_VALGRIND "Enable running the tests using Valgrind" ${DEFAULT_VALGRIND})
option(NO_ENABLE_ADDITIONAL_TESTS "Enable running tests that require full visibility on the hotrod library" ${DEFAULT_ADDITIONAL_TESTS})

if(DEFINED HOTROD_PREBUILT_LIB_DIR)
    set(PROTOBUF_GENERATE_CPP_APPEND_PATH true)
    set (INCLUDE_FILES_DIR ${HOTROD_PREBUILT_LIB_DIR}/../include)
    include_directories(${INCLUDE_FILES_DIR})

    # Find hotrod libraries looking only into the distro pack
    find_library(HOTROD_LIBRARY NAMES hotrod PATHS ${HOTROD_PREBUILT_LIB_DIR} NO_DEFAULT_PATH)
    if("${HOTROD_LIBRARY}" STREQUAL "HOTROD_LIBRARY-NOTFOUND")
        message(FATAL_ERROR "Cannot find HotRod dynamic library in directory '${HOTROD_PREBUILT_LIB_DIR}'.")
    else("${HOTROD_LIBRARY}" STREQUAL "HOTROD_LIBRARY-NOTFOUND")
        message("-- Found HotRod dynamic library: ${HOTROD_LIBRARY}")
        add_library(hotrod SHARED IMPORTED GLOBAL)
        set_target_properties(hotrod PROPERTIES IMPORTED_LOCATION ${HOTROD_LIBRARY})
        set_target_properties(hotrod PROPERTIES IMPORTED_IMPLIB ${HOTROD_LIBRARY})
        if(WIN32)
            # Copy pre-built libraries to the build directory
            add_custom_target(copyhrlibs ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${HOTROD_PREBUILT_LIB_DIR} ${CMAKE_CFG_INTDIR})
        endif(WIN32)
    endif("${HOTROD_LIBRARY}" STREQUAL "HOTROD_LIBRARY-NOTFOUND")

    # Find hotrod_protobuf static libraries looking only into the distro pack
    find_library(HOTROD_PROTOBUF_LIBRARY NAMES hotrod_protobuf PATHS ${HOTROD_PREBUILT_LIB_DIR} NO_DEFAULT_PATH)
    if("${HOTROD_PROTOBUF_LIBRARY}" STREQUAL "HOTROD_PROTOBUF_LIBRARY-NOTFOUND")
        message(FATAL_ERROR "Cannot find HotRod Protobuf library in directory '${HOTROD_PREBUILT_LIB_DIR}'.")
    else("${HOTROD_PROTOBUF_LIBRARY}" STREQUAL "HOTROD_PROTOBUF_LIBRARY-NOTFOUND")
        message("-- Found HotRod Protobuf library: ${HOTROD_PROTOBUF_LIBRARY}")
        add_library(hotrod_protobuf STATIC IMPORTED GLOBAL)
        set_target_properties(hotrod_protobuf PROPERTIES IMPORTED_LOCATION ${HOTROD_PROTOBUF_LIBRARY})
    endif("${HOTROD_PROTOBUF_LIBRARY}" STREQUAL "HOTROD_PROTOBUF_LIBRARY-NOTFOUND")

    # Find protobuf libraries looking only into the distro pack
    if(DEFINED HR_USE_SYSTEM_PROTOBUF)
        find_package(Protobuf REQUIRED)
        set(HR_PROTOBUF_LIBRARY ${PROTOBUF_LIBRARY})
    else(DEFINED HR_USE_SYSTEM_PROTOBUF)
        #FIND protoc executable
        if(UNIX)
            set (PROTOBUF_PROTOC_EXECUTABLE ${HOTROD_PREBUILT_LIB_DIR}/../bin/protoc)
        else(UNIX)
            set (PROTOBUF_PROTOC_EXECUTABLE ${HOTROD_PREBUILT_LIB_DIR}/../bin/protoc.exe)
        endif(UNIX)
        # look only into the distro pack
        find_library(HR_PROTOBUF_LIBRARY NAMES libprotobuf.lib protobuf  PATHS ${HOTROD_PREBUILT_LIB_DIR}
            NO_SYSTEM_ENVIRONMENT_PATH NO_DEFAULT_PATH
            NO_CMAKE_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_DEFAULT_PATH)
    endif(DEFINED HR_USE_SYSTEM_PROTOBUF)
    message("-- Found protoc executable: ${PROTOBUF_PROTOC_EXECUTABLE}")
    if("${HR_PROTOBUF_LIBRARY}" STREQUAL "HR_PROTOBUF_LIBRARY-NOTFOUND")
        message(FATAL_ERROR "Cannot find protobuf static library in directory '${HOTROD_PREBUILT_LIB_DIR}'.")
    else("${HR_PROTOBUF_LIBRARY}" STREQUAL "HR_PROTOBUF_LIBRARY-NOTFOUND")
        message("-- Found Protobuf library: ${HR_PROTOBUF_LIBRARY}")
        add_library(protobuf IMPORTED GLOBAL)
        set_target_properties(protobuf PROPERTIES IMPORTED_LOCATION ${HR_PROTOBUF_LIBRARY})
        set_target_properties(protobuf PROPERTIES IMPORTED_IMPLIB ${HR_PROTOBUF_LIBRARY})
        set(PROTOBUF_LIBRARY ${HR_PROTOBUF_LIBRARY})
    endif("${HR_PROTOBUF_LIBRARY}" STREQUAL "HR_PROTOBUF_LIBRARY-NOTFOUND")

    # Find OpenSSL
    if(NOT DEFINED WIN32)
        find_package(OpenSSL)
        include_directories(${OPENSSL_INCLUDE_DIR})
    endif(NOT DEFINED WIN32)


    set (PROTOBUF_INCLUDE_DIR ${HOTROD_PREBUILT_LIB_DIR}/../include)
else(DEFINED HOTROD_PREBUILT_LIB_DIR)
    find_package(Protobuf REQUIRED)

    # OpenSSL support
    if(NOT DEFINED WIN32)
        find_package(OpenSSL)
        include_directories(${OPENSSL_INCLUDE_DIR})
    endif(NOT DEFINED WIN32)
    include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_BINARY_DIR}")
    set (INCLUDE_FILES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)


    # Select driver
    if(HOTROD_WINAPI)
      set (platform_sources src/hotrod/sys/windows/Socket.cpp src/hotrod/sys/windows/Thread.cpp
               src/hotrod/sys/windows/platform.cpp src/hotrod/sys/windows/Inet.cpp src/hotrod/sys/windows/Time.cpp)
    else(HOTROD_WINAPI)
      set (platform_sources src/hotrod/sys/posix/Socket.cpp src/hotrod/sys/posix/Thread.cpp
               src/hotrod/sys/posix/platform.cpp src/hotrod/sys/posix/Mutex.cpp src/hotrod/sys/posix/Inet.cpp src/hotrod/sys/posix/Time.cpp)
    endif(HOTROD_WINAPI)

    if(ENABLE_INTERNAL_TESTING)
    set (internal_test_sources
      src/hotrod/test/Unit.cpp
      src/hotrod/test/HashTest.cpp
      src/hotrod/test/ConnectionPoolTest.cpp
    )
    endif(ENABLE_INTERNAL_TESTING)
    if(WIN32)
    set (socket_impl
          src/hotrod/impl/transport/tcp/SChannelTcpTransport.cpp
          src/hotrod/sys/SChannelSocket.cpp
          )
    else(WIN32)
    set (socket_impl
          src/hotrod/impl/transport/tcp/SSLTcpTransport.cpp
          src/hotrod/sys/SSLSocket.cpp
          )
    endif(WIN32)

    set (library_sources
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
      ${internal_test_sources}  
      ${CMAKE_BINARY_DIR}/Version.cpp
    )
    hr_protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS infinispan/hotrod
      proto/infinispan/hotrod/protobuf/base_types.proto
      proto/org/infinispan/protostream/message-wrapping.proto
      proto/org/infinispan/query/remote/client/query.proto
    )

    include_directories(${PROTOBUF_INCLUDE_DIR})
    
    add_library (hotrod_protobuf STATIC ${PROTO_SRCS})
    set_target_properties(hotrod_protobuf PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}" )
    if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set_target_properties(hotrod_protobuf PROPERTIES COMPILE_FLAGS "-fPIC ${WARNING_FLAGS}" )
    endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_include_directories(hotrod_protobuf PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                     "${INCLUDE_FILES_DIR}"
                                     "${CMAKE_CURRENT_BINARY_DIR}"
                                     "${PROTOBUF_INCLUDE_DIR}")

    configure_file(src/hotrod/impl/Version.cpp.in ${CMAKE_BINARY_DIR}/Version.cpp @ONLY)
    if (MSVC)
        set_target_properties(hotrod_protobuf PROPERTIES COMPILE_FLAGS "/wd4244 /wd4267" )
    endif (MSVC)
    
    # Build a shared library
    add_library (hotrod SHARED ${library_sources})

    set_target_properties(hotrod PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
    set_target_properties(hotrod PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS} ${STATIC_FLAGS}")
    target_link_libraries (hotrod ${platform_libs}  ${OPENSSL_LIBRARIES} hotrod_protobuf ${PROTOBUF_LIBRARY})

    if (WIN32 AND NOT CMAKE_SIZEOF_VOID_P MATCHES "8")
        set (OUTPUT_NAME_SUFFIX "32")
    else (WIN32 AND NOT CMAKE_SIZEOF_VOID_P MATCHES "8")
        set (OUTPUT_NAME_SUFFIX "")
    endif (WIN32 AND NOT CMAKE_SIZEOF_VOID_P MATCHES "8")

    set_target_properties (hotrod PROPERTIES OUTPUT_NAME "hotrod${OUTPUT_NAME_SUFFIX}")
    set_target_properties (hotrod PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
    set_target_properties (hotrod PROPERTIES LINK_FLAGS "${CATCH_UNDEFINED}")
    set_target_properties (hotrod PROPERTIES SOVERSION "1.0")

    # Build a static library

    add_custom_target(build_dist COMMAND "echo" "build_dist completed")
    add_dependencies(build_dist hotrod docs)
endif(DEFINED HOTROD_PREBUILT_LIB_DIR)
if (NOT DEFINED COMPILER_FLAGS)
    message(FATAL_ERROR "Compiler flags not set for this build type")
endif (NOT DEFINED COMPILER_FLAGS)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS} ${WARNING_FLAGS}")

if (ENABLE_VALGRIND)
  find_program(VALGRIND valgrind)
  if (VALGRIND MATCHES .*-NOTFOUND)
    message(FATAL_ERROR "Cannot find valgrind in your environment.  Please install valgrind or use NOENABLE_VALGRIND")
  endif(VALGRIND MATCHES .*-NOTFOUND)
  set(MEMORYCHECK_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/valgrind CACHE FILEPATH
      "Hot Rod Valgrind preprocessor" FORCE)
  set(MEMORYCHECK_COMMAND_OPTIONS
      "${VALGRIND} --error-exitcode=1 --leak-check=full --num-callers=25 --workaround-gcc296-bugs=yes --demangle=yes"
      CACHE STRING "Valgrind options" FORCE)
  configure_file ("${PROJECT_SOURCE_DIR}/CTestCustom.cmake" "${PROJECT_BINARY_DIR}/CTestCustom.cmake" COPYONLY)
endif (ENABLE_VALGRIND)

file(COPY ${CMAKE_SOURCE_DIR}/test/query_proto DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

hr_protobuf_generate_cpp(TEST_PROTO_SRCS TEST_PROTO_HDRS .
  test/query_proto/addressbook.proto
  test/query_proto/bank.proto
)

find_package(Java)

add_custom_target(build_test COMMAND "echo" "build_test completed")
add_dependencies(build_test ${JAVA_TESTS} krbserver unittest simple queryTest events
                            nearCacheTest nearCacheFailoverTest continuousQueryTest
                            simpleSasl simple-tls simple-tls-sni simpleSaslTls PutGetTest xunitQueryTest ClearTest xunit_nearCacheTest 
                            CountersTest simpleTx transcoder TranscoderTest TransactionTest poolTest)

add_custom_target(build_native_test COMMAND "echo" "build_native_test completed")
add_dependencies(build_native_test unittest simple queryTest events
                            nearCacheTest continuousQueryTest
                            simpleSasl simple-tls simple-tls-sni simpleSaslTls  transcoder docs)

# TESTS

if (DEFINED ENABLE_ADDITIONAL_TESTS)  # Compile a version with all symbols for additional tests with gcc
    add_executable (poolUnitTest test/PoolUnitTest.cpp)
    target_include_directories(poolUnitTest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                         "${INCLUDE_FILES_DIR}"
                                         "${CMAKE_CURRENT_BINARY_DIR}"
                                         "${PROTOBUF_INCLUDE_DIR}")
    set_property(TARGET poolUnitTest PROPERTY CXX_STANDARD 11)
    set_property(TARGET poolUnitTest PROPERTY CXX_STANDARD_REQUIRED ON)
    set_target_properties(poolUnitTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
    set_target_properties (poolUnitTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
    target_link_libraries (poolUnitTest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})
endif (DEFINED ENABLE_ADDITIONAL_TESTS)

add_executable (simple test/Simple.cpp)
target_include_directories(simple PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                     "${INCLUDE_FILES_DIR}"
                                     "${CMAKE_CURRENT_BINARY_DIR}"
                                     "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET simple PROPERTY CXX_STANDARD 11)
set_property(TARGET simple PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(simple PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties (simple PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries (simple hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable (poolTest test/PoolTest.cpp)
target_include_directories(poolTest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                     "${INCLUDE_FILES_DIR}"
                                     "${CMAKE_CURRENT_BINARY_DIR}"
                                     "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET poolTest PROPERTY CXX_STANDARD 11)
set_property(TARGET poolTest PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(poolTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties (poolTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries (poolTest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable (queryTest test/QueryTest.cpp ${TEST_PROTO_SRCS})
target_include_directories(queryTest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                     "${INCLUDE_FILES_DIR}"
                                     "${INCLUDE_FILES_DIR}/infinispan/hotrod"
                                     "${CMAKE_CURRENT_BINARY_DIR}"
                                     "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET queryTest PROPERTY CXX_STANDARD 11)
set_property(TARGET queryTest PROPERTY CXX_STANDARD_REQUIRED ON)
if (MSVC)
    set_target_properties (queryTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS} /wd4244 /wd4267")
else (MSVC)
    set_target_properties (queryTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
endif (MSVC)
set_target_properties(queryTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
target_link_libraries (queryTest hotrod hotrod_protobuf  ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable (continuousQueryTest test/ContinuousQueryTest.cpp ${TEST_PROTO_SRCS})
target_include_directories(continuousQueryTest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                     "${INCLUDE_FILES_DIR}"
                                     "${INCLUDE_FILES_DIR}/infinispan/hotrod"
                                     "${CMAKE_CURRENT_BINARY_DIR}"
                                     "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET continuousQueryTest PROPERTY CXX_STANDARD 11)
set_property(TARGET continuousQueryTest PROPERTY CXX_STANDARD_REQUIRED ON)
if (MSVC)
    set_target_properties (continuousQueryTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS} /wd4244 /wd4267")
else (MSVC)
    set_target_properties (continuousQueryTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
endif (MSVC)
set_target_properties(continuousQueryTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
target_link_libraries (continuousQueryTest hotrod hotrod_protobuf  ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable (events test/Events.cpp)
target_include_directories(events PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                     "${INCLUDE_FILES_DIR}"
                                     "${CMAKE_CURRENT_BINARY_DIR}"
                                     "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET events PROPERTY CXX_STANDARD 11)
set_property(TARGET events PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(events PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties (events PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries (events hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable (nearCacheTest test/NearCacheTest.cpp)
target_include_directories(nearCacheTest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                     "${INCLUDE_FILES_DIR}"
                                     "${CMAKE_CURRENT_BINARY_DIR}"
                                     "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET nearCacheTest PROPERTY CXX_STANDARD 11)
set_property(TARGET nearCacheTest PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(nearCacheTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties (nearCacheTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries (nearCacheTest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable (nearCacheFailoverTest test/NearCacheFailoverTest.cpp)
target_include_directories(nearCacheFailoverTest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                     "${INCLUDE_FILES_DIR}"
                                     "${CMAKE_CURRENT_BINARY_DIR}"
                                     "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET nearCacheFailoverTest PROPERTY CXX_STANDARD 11)
set_property(TARGET nearCacheFailoverTest PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(nearCacheFailoverTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties (nearCacheFailoverTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries (nearCacheFailoverTest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

    add_executable (simpleSasl test/SimpleSasl.cpp)
    target_include_directories(simpleSasl PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                         "${INCLUDE_FILES_DIR}"
                                         "${CMAKE_CURRENT_BINARY_DIR}"
                                         "${PROTOBUF_INCLUDE_DIR}")
    set_property(TARGET simpleSasl PROPERTY CXX_STANDARD 11)
    set_property(TARGET simpleSasl PROPERTY CXX_STANDARD_REQUIRED ON)
    set_target_properties(simpleSasl PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
    set_target_properties (simpleSasl PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
    target_link_libraries (simpleSasl hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})
    add_executable (simple-tls test/SimpleTLS.cpp)
    target_include_directories(simple-tls PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                         "${INCLUDE_FILES_DIR}"
                                         "${INCLUDE_FILES_DIR}/infinispan/hotrod"
                                         "${CMAKE_CURRENT_BINARY_DIR}"
                                         "${PROTOBUF_INCLUDE_DIR}")
    set_property(TARGET simple-tls PROPERTY CXX_STANDARD 11)
    set_property(TARGET simple-tls PROPERTY CXX_STANDARD_REQUIRED ON)
    set_target_properties(simple-tls PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
    set_target_properties (simple-tls PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
    target_link_libraries (simple-tls hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs} ${OPENSSL_LIBRARIES})

    add_executable (simpleSaslTls test/SimpleSaslTls.cpp)
    target_include_directories(simpleSaslTls PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                         "${INCLUDE_FILES_DIR}"
                                         "${CMAKE_CURRENT_BINARY_DIR}"
                                         "${PROTOBUF_INCLUDE_DIR}")
    set_property(TARGET simpleSaslTls PROPERTY CXX_STANDARD 11)
    set_property(TARGET simpleSaslTls PROPERTY CXX_STANDARD_REQUIRED ON)
    set_target_properties(simpleSaslTls PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
    set_target_properties (simpleSaslTls PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
    target_link_libraries (simpleSaslTls hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

    add_executable (simple-tls-sni test/SimpleTLS-SNI.cpp)
    target_include_directories(simple-tls-sni PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                         "${INCLUDE_FILES_DIR}"
                                         "${INCLUDE_FILES_DIR}/infinispan/hotrod"
                                         "${CMAKE_CURRENT_BINARY_DIR}"
                                         "${PROTOBUF_INCLUDE_DIR}")
    set_property(TARGET simple-tls-sni PROPERTY CXX_STANDARD 11)
    set_property(TARGET simple-tls-sni PROPERTY CXX_STANDARD_REQUIRED ON)
    set_target_properties(simple-tls-sni PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
    set_target_properties (simple-tls-sni PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
    target_link_libraries (simple-tls-sni hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs} ${OPENSSL_LIBRARIES})

if (ENABLE_INTERNAL_TESTING)
    add_executable (unittest test/Unit.cpp)
target_include_directories(unittest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                     "${INCLUDE_FILES_DIR}"
                                     "${INCLUDE_FILES_DIR}/infinispan/hotrod"
                                     "${CMAKE_CURRENT_BINARY_DIR}"
                                     "${PROTOBUF_INCLUDE_DIR}")
    set_target_properties(unittest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}") 
    set_target_properties (unittest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS}")
    target_link_libraries (unittest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY})

endif (ENABLE_INTERNAL_TESTING)

add_executable (itest test/InteractiveTest.cpp)
target_include_directories(itest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                     "${INCLUDE_FILES_DIR}"
                                     "${CMAKE_CURRENT_BINARY_DIR}"
                                     "${PROTOBUF_INCLUDE_DIR}")
target_include_directories(itest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto")
set_target_properties(itest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties (itest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries (itest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${OPENSSL_LIBRARIES} ${platform_libs})

add_executable (simpleTx test/SimpleTx.cpp)
target_include_directories(simpleTx PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                     "${INCLUDE_FILES_DIR}"
                                     "${CMAKE_CURRENT_BINARY_DIR}"
                                     "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET simpleTx PROPERTY CXX_STANDARD 11)
set_property(TARGET simpleTx PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(simpleTx PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties (simpleTx PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries (simpleTx hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable (transcoder test/Transcoder.cpp)
target_include_directories(transcoder PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
                                     "${INCLUDE_FILES_DIR}"
                                     "${CMAKE_CURRENT_BINARY_DIR}"
                                     "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET transcoder PROPERTY CXX_STANDARD 11)
set_property(TARGET transcoder PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(transcoder PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties (transcoder PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries (transcoder hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

# the CTest include must be after the MEMORYCHECK settings are processed
include (CTest)
find_package(PythonInterp)
if (NOT DEFINED PythonInterp_FOUND)
      message(WARNING "Python interpreter not found. This could be ok if you don't need tests")
endif (NOT DEFINED PythonInterp_FOUND)

set (CMAKE_ROOT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT DEFINED HOTROD_JBOSS_HOME)
   if (NOT DEFINED ENV{JBOSS_HOME})
      message(WARNING "you must set the JBOSS_HOME environment variable or use -DHOTROD_JBOSS_HOME=/the/path if you want to run integration tests")
   else (NOT DEFINED ENV{JBOSS_HOME})
      set(HOTROD_JBOSS_HOME $ENV{JBOSS_HOME} CACHE FILEPATH "Infinispan HOME dir")
   endif (NOT DEFINED ENV{JBOSS_HOME})
endif (NOT DEFINED HOTROD_JBOSS_HOME)

ADD_SUBDIRECTORY(xunit-test)

if (ENABLE_INTERNAL_TESTING)
    add_test (unittest unittest)
endif (ENABLE_INTERNAL_TESTING)    

if (DEFINED ENABLE_ADDITIONAL_TESTS)  # Run unit tests that require full symbols visibility
    add_test (poolUnitTest poolUnitTest)
endif (DEFINED ENABLE_ADDITIONAL_TESTS)

find_program(MVN_PROGRAM "mvn")
if (MVN_PROGRAM STREQUAL "MVN_PROGRAM-NOTFOUND")
    message(WARNING "Apache Maven (mvn) not found in path")
endif (MVN_PROGRAM STREQUAL "MVN_PROGRAM-NOTFOUND")

if(DEFINED maven.version.org.infinispan)
  set(MVN_ISPN_VER_OPT "-Dversion.org.infinispan=${maven.version.org.infinispan}")
endif(DEFINED maven.version.org.infinispan)

if(DEFINED maven.settings.file)
  set(MVN_SETTINGS_FILE_OPT -s ${maven.settings.file})
  set(MVN_SETTINGS_EXEC_FILE_OPT "-s ${maven.settings.file}")
else(DEFINED maven.settings.file)
  set(MVN_SETTINGS_FILE_OPT "")
  set(MVN_SETTINGS_EXEC_FILE_OPT "")
endif(DEFINED maven.settings.file)

set(KRBSERVER_DIR "${CMAKE_CURRENT_BINARY_DIR}/krbserver")
add_custom_command(OUTPUT ${KRBSERVER_DIR}/target/classes/krbserver/Server.class
    COMMAND ${MVN_PROGRAM}
    ARGS "-B" ${MVN_SETTINGS_FILE_OPT} ${MVN_ISPN_VER_OPT} "package"
    WORKING_DIRECTORY "${KRBSERVER_DIR}"
)

add_custom_target(krbserver ALL DEPENDS ${KRBSERVER_DIR}/target/classes/krbserver/Server.class)

if (NOT ((EXISTS "${HOTROD_JBOSS_HOME}/bin/server.sh") AND (EXISTS "${HOTROD_JBOSS_HOME}/bin/server.bat")))
    message(WARNING "JBOSS_HOME ${HOTROD_JBOSS_HOME} does not have needed startup scripts")
else (NOT ((EXISTS "${HOTROD_JBOSS_HOME}/bin/server.sh") AND (EXISTS "${HOTROD_JBOSS_HOME}/bin/server.bat")))
    if (NOT Java_JAVA_EXECUTABLE)
        message(FATAL_ERROR "Java not found")
    endif (NOT Java_JAVA_EXECUTABLE)
    file(COPY ${CMAKE_SOURCE_DIR}/krbserver DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    file(COPY ${CMAKE_SOURCE_DIR}/test/data/krb5.conf DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    add_test (start_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_JBOSS_HOME} infinispan.xml)
    add_test (probe_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60)
    add_test (simple simple)
    add_test (transcoder transcoder)
    add_test (queryTest queryTest)
    add_test (events events)
    add_test (nearCacheTest nearCacheTest)
    add_test (continuousQueryTest continuousQueryTest)
    add_test (simpleTx simpleTx)
    add_test (poolTest poolTest)
    add_test (simple-mediatype simple 2.8)
    add_test (queryTest-mediatype queryTest 2.8)
    add_test (events-mediatype-mediatype events 2.8)
    add_test (nearCacheTest-mediatype nearCacheTest 2.8)
    add_test (continuousQueryTest-mediatype continuousQueryTest 2.8)
    add_test (simpleTx-mediatype simpleTx 2.8)
    add_test (stop_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py stop)
    add_test (probe_port_stop ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60 down)
    add_test (start_cluster_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_JBOSS_HOME} infinispan.xml)
    add_test (probe_cluster_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60)
    add_test (start_fail_over_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_JBOSS_HOME} "infinispan.xml" "-o 100 -s ${HOTROD_JBOSS_HOME}/server1" server2.pkl)
    add_test (probe_fail_over_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11322 60)
    add_test (nearCacheFailover nearCacheFailoverTest ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py server2.pkl)
    add_test (probe_fail_over_port_stop ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11322 60 down)
    add_test (stop_cluster_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py stop)
    add_test (probe_cluster_port_stop ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60 down)
    if (WIN32)
        add_test (start_sals_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_JBOSS_HOME} infinispan-sasl.xml)
        add_test (probe_sasl_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60)
        add_test (simpleSasl simpleSasl)
        add_test (stop_sasl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py stop)
        add_test (probe_port_stop_sasl ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60 down)
    else (WIN32)
        add_test (start_krbserver  ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/start_and_probe_port.py localhost 10389 60 "${MVN_PROGRAM} ${MVN_SETTINGS_EXEC_FILE_OPT} -f krbserver/pom.xml exec:java -Dexec.mainClass=krbserver.Server")
        add_test (start_sals_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_JBOSS_HOME} infinispan-sasl.xml)
        set_tests_properties(start_sals_server PROPERTIES
                  ENVIRONMENT JAVA_OPTS="-Djava.security.krb5.conf=${CMAKE_CURRENT_BINARY_DIR}/krb5.conf -Djava.security.krb5.debug=true -Djboss.security.disable.secdomain.option=true")
        add_test (probe_sasl_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60)
        add_test (simpleSasl simpleSasl)
        add_test (stop_sasl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py stop)
        add_test (probe_port_stop_sasl ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60 down)
        add_test (stop_krbserver ${MVN_PROGRAM} ${MVN_SETTINGS_FILE_OPT} -f krbserver/pom.xml exec:java -Dexec.mainClass=krbserver.Server -Dexec.args=stop)
    endif(WIN32)
    add_test (start_ssl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_JBOSS_HOME} infinispan-ssl.xml)
    add_test (probe_ssl_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60)
    add_test (simple-tls simple-tls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/infinispan-ca.pem)
    #    add_test (simple-tls-sni simple-tls-sni --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/keystore_server_cert.pem)
    add_test (simple-tls-mediatype simple-tls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/infinispan-ca.pem --protocol_version 2.8)
    #    add_test (simple-tls-sni-mediatype simple-tls-sni --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/keystore_server_cert.pem --protocol_version 2.8)
    add_test (stop_ssl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py stop)
    add_test (probe_port_stop_ssl ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60 down)
    add_test (start_ssl_server_client_auth ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_JBOSS_HOME} infinispan-auth-ssl.xml)
    add_test (probe_ssl_port_client_auth ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60)
    if (WIN32)    
        add_test (simple-tls-client-auth simple-tls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/truststore_client.p12)
	#    add_test (simple-tls-sni-client-auth simple-tls-sni --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/keystore_server_cert.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/truststore_client.p12)
        add_test (simple-tls-client-auth simple-tls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/truststore_client.p12 --protocol_version 2.8)
	#    add_test (simple-tls-sni-client-auth simple-tls-sni --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/keystore_server_cert.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/truststore_client.p12 --protocol_version 2.8)
        add_test (stop_ssl_server_client_auth ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py stop)
        add_test (start_sals_ssl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_JBOSS_HOME} infinispan-sasl-ssl.xml)
        add_test (probe_sasl_ssl_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60)
        add_test (simpleSaslTls simpleSaslTls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/truststore_client.p12)
        add_test (stop_sasl_ssl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py stop)
    else (WIN32)
        add_test (simple-tls-client-auth simple-tls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/truststore_client.pem)
	#    add_test (simple-tls-sni-client-auth simple-tls-sni --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/keystore_server_cert.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/truststore_client.pem)
        add_test (simple-tls-client-auth-mediatype simple-tls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/truststore_client.pem --protocol_version 2.8)
	#    add_test (simple-tls-sni-client-auth-mediatype simple-tls-sni --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/keystore_server_cert.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/truststore_client.pem --protocol_version 2.8)
        add_test (stop_ssl_server_client_auth ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py stop)
        add_test (start_sals_ssl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_JBOSS_HOME} infinispan-sasl-ssl.xml)
        add_test (probe_sasl_ssl_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60)
        add_test (simpleSaslTls simpleSaslTls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/truststore_client.pem)
        add_test (simpleSaslTls-mediatype simpleSaslTls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/test/data/truststore_client.pem --protocol_version 2.8)
        add_test (stop_sasl_ssl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py stop)
    endif(WIN32)
endif (NOT ((EXISTS "${HOTROD_JBOSS_HOME}/bin/server.sh") AND (EXISTS "${HOTROD_JBOSS_HOME}/bin/server.bat")))

if (ENABLE_VALGRIND)
    add_custom_target(memtest ${CMAKE_CTEST_COMMAND} -D ExperimentalMemCheck
        COMMENT "Running CTest with Valgrind options")

    add_custom_target(memtestv ${CMAKE_CTEST_COMMAND} -V -D ExperimentalMemCheck
        COMMENT "Running CTest in verbose mode with Valgrind options")
endif (ENABLE_VALGRIND)

if(NOT DEFINED HOTROD_PREBUILT_LIB_DIR)
set (CPACK_GENERATOR "ZIP")
set (CPACK_SOURCE_GENERATOR "ZIP")
set (CPACK_INCLUDE_TOPLEVEL_DIRECTORY "1")
set (CPACK_SOURCE_IGNORE_FILES "/CVS/;/.svn/;/.bzr/;/.hg/;/.git/;.swp$;.#;/#;${CMAKE_BINARY_DIR}/*;/infinispan-server-*")
if (CMAKE_SIZEOF_VOID_P MATCHES "8")
    set (PACKAGE_ARCH "x86_64")
else (CMAKE_SIZEOF_VOID_P MATCHES "8")
    set (PACKAGE_ARCH "i686")
endif (CMAKE_SIZEOF_VOID_P MATCHES "8")

if(WIN32)
    set (PLATFORM "WIN")
elseif (APPLE)
    set (PLATFORM "OSX")
else (WIN32)
    if(EXISTS "/etc/redhat-release")
        set (PLATFORM "RHEL")
    elseif(EXISTS "/etc/debian_version")
        set (PLATFORM "DEB") 
    endif(EXISTS "/etc/redhat-release")
endif(WIN32)
message (STATUS "Build ${CMAKE_BUILD_TYPE} for ${PLATFORM} on ${PACKAGE_ARCH}")
set (CPACK_SYSTEM_NAME "${PLATFORM}-${PACKAGE_ARCH}")
#set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
#set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
if(WIN32)
set (DOCDIR .)
else(WIN32)
    set (DOCDIR ${SHARE_INSTALL_PREFIX}/doc/${CMAKE_PROJECT_NAME})
endif(WIN32)
message (STATUS "OpenSSL support ${OPENSSL_FOUND}")

file (GLOB includes "${CMAKE_CURRENT_SOURCE_DIR}/include/infinispan/hotrod/*.h")
install (FILES ${includes} DESTINATION include/infinispan/hotrod)
install (FILES ${PROTO_HDRS} DESTINATION include/infinispan/hotrod)
install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/License.txt" "${CMAKE_CURRENT_SOURCE_DIR}/dist/README.md" DESTINATION ${DOCDIR})

# Include protobuf library in the distribution
install (DIRECTORY ${PROTOBUF_INCLUDE_DIRS}/google/protobuf DESTINATION include/google)
get_filename_component(real_protobuf_library "${PROTOBUF_LIBRARY}" REALPATH)
get_filename_component(real_protoc_library "${PROTOBUF_PROTOC_LIBRARY}" REALPATH)
install (FILES ${real_protobuf_library} ${real_protoc_library} ${PROTOBUF_LIBRARY} ${PROTOBUF_PROTOC_LIBRARY} DESTINATION lib)
install (FILES ${PROTOBUF_PROTOC_EXECUTABLE}
         DESTINATION bin
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ)
if (NOT WIN32)
    get_filename_component(protobuf_dir "${PROTOBUF_LIBRARY}" PATH)
    file(GLOB libproto_glob "${protobuf_dir}/libproto*")
    install (FILES ${libproto_glob} DESTINATION lib${LIB_SUFFIX})
endif (NOT WIN32)

if(NOT DEFINED HOTROD_PREBUILT_LIB_DIR)
    set_property(TARGET hotrod PROPERTY CXX_STANDARD 11)
    set_property(TARGET hotrod PROPERTY CXX_STANDARD_REQUIRED ON)
    install (TARGETS hotrod hotrod_protobuf DESTINATION lib${LIB_SUFFIX})
endif(NOT DEFINED HOTROD_PREBUILT_LIB_DIR)

include (CPack)

# API DOCS -- use target docs (i.e. make docs) from build directory to make api docs
# Toggle BUILD_DOCS to turn on/off the generation of the API documentation
FIND_PACKAGE(Doxygen)
if (DOXYGEN_FOUND)
  #-- Configure the Template Doxyfile for our specific project
  configure_file(Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
  # Delete any previously generated docs
  if(EXISTS ${PROJECT_SOURCE_DIR}/documentation/api_docs)
      file(REMOVE_RECURSE ${PROJECT_SOURCE_DIR}/documentation/api_docs)
  endif(EXISTS ${PROJECT_SOURCE_DIR}/documentation/api_docs)
  #-- Add a custom target to run Doxygen when ever the project is built
  add_custom_target (docs ALL
                     COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
                     SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
  #Include the API docs in the package.
  install (FILES ${PROJECT_SOURCE_DIR}/documentation/api_docs/html/ DESTINATION ${DOCDIR}/api)
endif (DOXYGEN_FOUND)
endif(NOT DEFINED HOTROD_PREBUILT_LIB_DIR)
