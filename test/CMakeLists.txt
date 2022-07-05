cmake_minimum_required(VERSION 3.0.0)
project(infinispan-hotrod-test-cpp VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(PythonInterp)

if(NOT DEFINED PythonInterp_FOUND)
    message(WARNING "Python interpreter not found. This could be ok if you don't need tests")
endif(NOT DEFINED PythonInterp_FOUND)

find_package(Java)

include(ExternalProject)

# Download and unpack googletest at configure time
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(GTEST_VER "release-1.8.1")
else("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(GTEST_VER "release-1.11.0")
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

configure_file(gtest-CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)

if(result)
    message(WARNING "Build step for googletest failed: ${result}")
    message(WARNING "If you don't need the xunit test suite, this should be ok.")
else()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)

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
        if(CMAKE_VERSION VERSION_LESS 2.8.11)
            include_directories("${gtest_SOURCE_DIR}/include")
        endif()
    endif()
endif()

set(CMAKE_ROOT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT DEFINED HOTROD_ISPN_HOME)
    if(NOT DEFINED ENV{ISPN_HOME})
        message(WARNING "you must set the ISPN_HOME environment variable or use -DHOTROD_ISPN_HOME=/the/path if you want to run integration tests")
    else(NOT DEFINED ENV{ISPN_HOME})
        set(HOTROD_ISPN_HOME $ENV{ISPN_HOME} CACHE FILEPATH "Infinispan HOME dir")
    endif(NOT DEFINED ENV{ISPN_HOME})
endif(NOT DEFINED HOTROD_ISPN_HOME)

set(DLLEXPORT "HR_PROTO_EXPORT=__attribute__ ((visibility (\"default\")))")
set(DLLEXPORT_STATIC "HR_PROTO_EXPORT= ")

set(HR_USE_SYSTEM_PROTOBUF TRUE)

if(DEFINED HR_USE_SYSTEM_PROTOBUF)
    # Find protobuf on the system
    find_package(Protobuf REQUIRED)
    set(HR_PROTOBUF_LIBRARY ${PROTOBUF_LIBRARY})
else(DEFINED HR_USE_SYSTEM_PROTOBUF)
    # Find protobuf libraries looking only into the distro pack
    # FIND protoc executable
    if(UNIX)
        set(PROTOBUF_PROTOC_EXECUTABLE ${HOTROD_PREBUILT_LIB_DIR}/../bin/protoc)
    else(UNIX)
        set(PROTOBUF_PROTOC_EXECUTABLE ${HOTROD_PREBUILT_LIB_DIR}/../bin/protoc.exe)
    endif(UNIX)

    # look only into the distro pack
    find_library(HR_PROTOBUF_LIBRARY NAMES libprotobuf.lib protobuf PATHS ${HOTROD_PREBUILT_LIB_DIR}
        NO_SYSTEM_ENVIRONMENT_PATH NO_DEFAULT_PATH
        NO_CMAKE_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_DEFAULT_PATH)
endif(DEFINED HR_USE_SYSTEM_PROTOBUF)

if(HOTROD_PREBUILT_LIB_DIR)
    set(INCLUDE_FILES_DIR ${HOTROD_PREBUILT_LIB_DIR}/../include)
    set(PROTOBUF_INCLUDE_DIR ${HOTROD_PREBUILT_LIB_DIR}/../include)
    set(HOTROD_LIB_DIR ${HOTROD_PREBUILT_LIB_DIR})
else(HOTROD_PREBUILT_LIB_DIR)
    set(INCLUDE_FILES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    set(PROTOBUF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../build)
    set(HOTROD_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../build)
endif(HOTROD_PREBUILT_LIB_DIR)

set(PROTOBUF_GENERATE_CPP_APPEND_PATH true)

# Find hotrod libraries looking only into the distro pack
find_library(HOTROD_LIBRARY NAMES hotrod PATHS ${HOTROD_LIB_DIR} NO_DEFAULT_PATH)

if("${HOTROD_LIBRARY}" STREQUAL "HOTROD_LIBRARY-NOTFOUND")
    message(FATAL_ERROR "Cannot find HotRod dynamic library in directory '${HOTROD_PREBUILT_LIB_DIR}'.")
else("${HOTROD_LIBRARY}" STREQUAL "HOTROD_LIBRARY-NOTFOUND")
    message("-- Found HotRod dynamic library: ${HOTROD_LIBRARY}")
    add_library(hotrod SHARED IMPORTED GLOBAL)
    set_target_properties(hotrod PROPERTIES IMPORTED_LOCATION ${HOTROD_LIBRARY})
    set_target_properties(hotrod PROPERTIES IMPORTED_IMPLIB ${HOTROD_LIBRARY})

    if(WIN32)
        # Copy pre-built libraries to the build directory
        add_custom_target(copyhrlibs ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${HOTROD_LIB_DIR} ${CMAKE_CFG_INTDIR})
    endif(WIN32)
endif("${HOTROD_LIBRARY}" STREQUAL "HOTROD_LIBRARY-NOTFOUND")

# Find hotrod_protobuf static libraries looking only into the distro pack
find_library(HOTROD_PROTOBUF_LIBRARY NAMES hotrod_protobuf PATHS ${HOTROD_LIB_DIR} NO_DEFAULT_PATH)

if("${HOTROD_PROTOBUF_LIBRARY}" STREQUAL "HOTROD_PROTOBUF_LIBRARY-NOTFOUND")
    message(FATAL_ERROR "Cannot find HotRod Protobuf library in directory '${HOTROD_PREBUILT_LIB_DIR}'.")
else("${HOTROD_PROTOBUF_LIBRARY}" STREQUAL "HOTROD_PROTOBUF_LIBRARY-NOTFOUND")
    message("-- Found HotRod Protobuf library: ${HOTROD_PROTOBUF_LIBRARY}")
    add_library(hotrod_protobuf STATIC IMPORTED GLOBAL)
    set_target_properties(hotrod_protobuf PROPERTIES IMPORTED_LOCATION ${HOTROD_PROTOBUF_LIBRARY})
endif("${HOTROD_PROTOBUF_LIBRARY}" STREQUAL "HOTROD_PROTOBUF_LIBRARY-NOTFOUND")

if("${HR_PROTOBUF_LIBRARY}" STREQUAL "HR_PROTOBUF_LIBRARY-NOTFOUND")
    message(FATAL_ERROR "Cannot find protobuf static library in directory '${HOTROD_PREBUILT_LIB_DIR}'.")
else("${HR_PROTOBUF_LIBRARY}" STREQUAL "HR_PROTOBUF_LIBRARY-NOTFOUND")
    message("-- Found Protobuf library: ${HR_PROTOBUF_LIBRARY}")
    add_library(protobuf SHARED IMPORTED GLOBAL)
    set_target_properties(protobuf PROPERTIES IMPORTED_LOCATION ${HR_PROTOBUF_LIBRARY})
    set_target_properties(protobuf PROPERTIES IMPORTED_IMPLIB ${HR_PROTOBUF_LIBRARY})
    set(PROTOBUF_LIBRARY ${HR_PROTOBUF_LIBRARY})
endif("${HR_PROTOBUF_LIBRARY}" STREQUAL "HR_PROTOBUF_LIBRARY-NOTFOUND")

find_package(OpenSSL)
include_directories(${OPENSSL_INCLUDE_DIR})

file(COPY ${CMAKE_SOURCE_DIR}/query_proto DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

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

hr_protobuf_generate_cpp(TEST_PROTO_SRCS TEST_PROTO_HDRS .
    query_proto/addressbook.proto
    query_proto/bank.proto
)

set(internal_test_sources
    internalTest/Unit.cpp
    internalTest/HashTest.cpp
    internalTest/ConnectionPoolTest.cpp
)

add_library(hotrod_internal_test SHARED ${internal_test_sources})
set_target_properties(hotrod_internal_test PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(hotrod_internal_test PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS} ${STATIC_FLAGS}")
target_include_directories(hotrod_internal_test PUBLIC "../src"
    "${INCLUDE_FILES_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
add_executable(simple Simple.cpp)
set_property(TARGET simple PROPERTY CXX_STANDARD 11)
set_property(TARGET simple PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(simple PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(simple PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries(simple hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable(poolTest PoolTest.cpp)
target_include_directories(poolTest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET poolTest PROPERTY CXX_STANDARD 11)
set_property(TARGET poolTest PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(poolTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(poolTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries(poolTest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable(poolUnitTest PoolUnitTest.cpp)
target_include_directories(poolUnitTest PUBLIC "../src"
    "${CMAKE_CURRENT_BINARY_DIR}/test/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET poolUnitTest PROPERTY CXX_STANDARD 11)
set_property(TARGET poolUnitTest PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(poolUnitTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(poolUnitTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries(poolUnitTest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable(queryTest QueryTest.cpp ${TEST_PROTO_SRCS})
target_include_directories(queryTest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${INCLUDE_FILES_DIR}/infinispan/hotrod"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET queryTest PROPERTY CXX_STANDARD 11)
set_property(TARGET queryTest PROPERTY CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set_target_properties(queryTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS} /wd4244 /wd4267")
else(MSVC)
    set_target_properties(queryTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
endif(MSVC)

set_target_properties(queryTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
target_link_libraries(queryTest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable(continuousQueryTest ContinuousQueryTest.cpp ${TEST_PROTO_SRCS})
target_include_directories(continuousQueryTest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${INCLUDE_FILES_DIR}/infinispan/hotrod"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET continuousQueryTest PROPERTY CXX_STANDARD 11)
set_property(TARGET continuousQueryTest PROPERTY CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set_target_properties(continuousQueryTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS} /wd4244 /wd4267")
else(MSVC)
    set_target_properties(continuousQueryTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
endif(MSVC)

set_target_properties(continuousQueryTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
target_link_libraries(continuousQueryTest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable(events Events.cpp)
target_include_directories(events PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET events PROPERTY CXX_STANDARD 11)
set_property(TARGET events PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(events PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(events PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries(events hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable(nearCacheTest NearCacheTest.cpp)
target_include_directories(nearCacheTest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET nearCacheTest PROPERTY CXX_STANDARD 11)
set_property(TARGET nearCacheTest PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(nearCacheTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(nearCacheTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries(nearCacheTest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable(nearCacheFailoverTest NearCacheFailoverTest.cpp)
target_include_directories(nearCacheFailoverTest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET nearCacheFailoverTest PROPERTY CXX_STANDARD 11)
set_property(TARGET nearCacheFailoverTest PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(nearCacheFailoverTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(nearCacheFailoverTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries(nearCacheFailoverTest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable(simpleSasl SimpleSasl.cpp)
target_include_directories(simpleSasl PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET simpleSasl PROPERTY CXX_STANDARD 11)
set_property(TARGET simpleSasl PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(simpleSasl PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(simpleSasl PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries(simpleSasl hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})
add_executable(simple-tls SimpleTLS.cpp)
target_include_directories(simple-tls PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${INCLUDE_FILES_DIR}/infinispan/hotrod"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET simple-tls PROPERTY CXX_STANDARD 11)
set_property(TARGET simple-tls PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(simple-tls PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(simple-tls PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries(simple-tls hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs} ${OPENSSL_LIBRARIES})

add_executable(simpleSaslTls SimpleSaslTls.cpp)
target_include_directories(simpleSaslTls PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET simpleSaslTls PROPERTY CXX_STANDARD 11)
set_property(TARGET simpleSaslTls PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(simpleSaslTls PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(simpleSaslTls PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries(simpleSaslTls hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable(simple-tls-sni SimpleTLS-SNI.cpp)
target_include_directories(simple-tls-sni PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${INCLUDE_FILES_DIR}/infinispan/hotrod"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET simple-tls-sni PROPERTY CXX_STANDARD 11)
set_property(TARGET simple-tls-sni PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(simple-tls-sni PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(simple-tls-sni PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries(simple-tls-sni hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs} ${OPENSSL_LIBRARIES})

add_executable(unittest Unit.cpp)
target_include_directories(unittest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${INCLUDE_FILES_DIR}/infinispan/hotrod"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_target_properties(unittest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(unittest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS}")
target_link_libraries(unittest hotrod hotrod_protobuf hotrod_internal_test ${PROTOBUF_LIBRARY})

add_executable(itest InteractiveTest.cpp)
target_include_directories(itest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
target_include_directories(itest PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto")
set_target_properties(itest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(itest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries(itest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${OPENSSL_LIBRARIES} ${platform_libs})

add_executable(simpleTx SimpleTx.cpp)
target_include_directories(simpleTx PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET simpleTx PROPERTY CXX_STANDARD 11)
set_property(TARGET simpleTx PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(simpleTx PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(simpleTx PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries(simpleTx hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

add_executable(transcoder Transcoder.cpp)
target_include_directories(transcoder PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/query_proto"
    "${INCLUDE_FILES_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROTOBUF_INCLUDE_DIR}")
set_property(TARGET transcoder PROPERTY CXX_STANDARD 11)
set_property(TARGET transcoder PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(transcoder PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}")
set_target_properties(transcoder PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
target_link_libraries(transcoder hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${platform_libs})

ADD_SUBDIRECTORY(xunit-test)

add_test(unittest unittest)
add_test(poolUnitTest poolUnitTest)

find_program(MVN_PROGRAM "mvn")

if(MVN_PROGRAM STREQUAL "MVN_PROGRAM-NOTFOUND")
    message(WARNING "Apache Maven (mvn) not found in path")
endif(MVN_PROGRAM STREQUAL "MVN_PROGRAM-NOTFOUND")

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

if(UNIX)
    set(KRBSERVER_DIR "${CMAKE_CURRENT_BINARY_DIR}/krbserver")
    add_custom_command(OUTPUT ${KRBSERVER_DIR}/target/classes/krbserver/Server.class
        COMMAND ${MVN_PROGRAM}
        ARGS "-B" ${MVN_SETTINGS_FILE_OPT} ${MVN_ISPN_VER_OPT} "package"
        WORKING_DIRECTORY "${KRBSERVER_DIR}"
    )

    add_custom_target(krbserver ALL DEPENDS ${KRBSERVER_DIR}/target/classes/krbserver/Server.class)
endif(UNIX)

if(NOT((EXISTS "${HOTROD_ISPN_HOME}/bin/server.sh") AND(EXISTS "${HOTROD_ISPN_HOME}/bin/server.bat")))
    message(WARNING "JBOSS_HOME ${HOTROD_ISPN_HOME} does not have needed startup scripts")
else(NOT((EXISTS "${HOTROD_ISPN_HOME}/bin/server.sh") AND(EXISTS "${HOTROD_ISPN_HOME}/bin/server.bat")))
    if(NOT Java_JAVA_EXECUTABLE)
        message(FATAL_ERROR "Java not found")
    endif(NOT Java_JAVA_EXECUTABLE)

    if(UNIX)
        file(COPY ${CMAKE_SOURCE_DIR}/krbserver DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
        file(COPY ${CMAKE_SOURCE_DIR}/data/krb5.conf DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    endif(UNIX)

    add_test(start_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_ISPN_HOME} infinispan.xml)
    add_test(probe_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60)
    add_test(simple simple)
    add_test(transcoder transcoder)
    add_test(queryTest queryTest)
    add_test(events events)
    add_test(nearCacheTest nearCacheTest)
    add_test(continuousQueryTest continuousQueryTest)
    add_test(simpleTx simpleTx)
    add_test(poolTest poolTest)
    add_test(simple-mediatype simple 2.8)
    add_test(queryTest-mediatype queryTest 2.8)
    add_test(events-mediatype-mediatype events 2.8)
    add_test(nearCacheTest-mediatype nearCacheTest 2.8)
    add_test(continuousQueryTest-mediatype continuousQueryTest 2.8)
    add_test(simpleTx-mediatype simpleTx 2.8)
    add_test(stop_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py stop)
    add_test(probe_port_stop ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60 down)
    add_test(start_cluster_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_ISPN_HOME} infinispan.xml)
    add_test(probe_cluster_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60)
    add_test(start_fail_over_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_ISPN_HOME} "infinispan.xml" "-o 100 -s ${HOTROD_ISPN_HOME}/server1" server2.pkl server1)
    add_test(probe_fail_over_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11322 60)
    add_test(nearCacheFailover nearCacheFailoverTest ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py server2.pkl)
    add_test(probe_fail_over_port_stop ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11322 60 down)
    add_test(stop_cluster_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py stop)
    add_test(probe_cluster_port_stop ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60 down)

    if(WIN32)
        add_test(start_sals_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_ISPN_HOME} infinispan-sasl.xml)
        add_test(probe_sasl_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60)
        add_test(simpleSasl simpleSasl)
        add_test(stop_sasl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py stop)
        add_test(probe_port_stop_sasl ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60 down)
    else(WIN32)
        add_test(start_krbserver ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/start_and_probe_port.py localhost 10389 60 "${MVN_PROGRAM} ${MVN_SETTINGS_EXEC_FILE_OPT} -f krbserver/pom.xml exec:java -Dexec.mainClass=krbserver.Server")
        add_test(start_sals_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_ISPN_HOME} infinispan-sasl.xml)
        set_tests_properties(start_sals_server PROPERTIES
            ENVIRONMENT JAVA_OPTS="-Djava.security.krb5.conf=${CMAKE_CURRENT_BINARY_DIR}/krb5.conf -Djava.security.krb5.debug=true -Djboss.security.disable.secdomain.option=true")
        add_test(probe_sasl_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60)
        add_test(simpleSasl simpleSasl)
        add_test(stop_sasl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py stop)
        add_test(probe_port_stop_sasl ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60 down)
        add_test(stop_krbserver ${MVN_PROGRAM} ${MVN_SETTINGS_FILE_OPT} -f krbserver/pom.xml exec:java -Dexec.mainClass=krbserver.Server -Dexec.args=stop)
    endif(WIN32)

    add_test(start_ssl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_ISPN_HOME} infinispan-ssl.xml)

    if(OPENSSL_VERSION VERSION_LESS "1.1.1")
        set_tests_properties(start_ssl_server PROPERTIES
            ENVIRONMENT JAVA_OPTS="-Dtest.useTLS=TLSv1.2")
    else(OPENSSL_VERSION VERSION_LESS "1.1.1")
        set_tests_properties(start_ssl_server PROPERTIES
            ENVIRONMENT JAVA_OPTS="-Dtest.useTLS=TLSv1.3")
    endif(OPENSSL_VERSION VERSION_LESS "1.1.1")

    add_test(probe_ssl_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60)
    add_test(simple-tls simple-tls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/infinispan-ca.pem)

    # add_test (simple-tls-sni simple-tls-sni --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/keystore_server_cert.pem)
    add_test(simple-tls-mediatype simple-tls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/infinispan-ca.pem --protocol_version 2.8)

    # add_test (simple-tls-sni-mediatype simple-tls-sni --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/keystore_server_cert.pem --protocol_version 2.8)
    add_test(stop_ssl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py stop)
    add_test(probe_port_stop_ssl ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60 down)
    add_test(start_ssl_server_client_auth ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_ISPN_HOME} infinispan-auth-ssl.xml)
    add_test(probe_ssl_port_client_auth ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60)

    if(WIN32)
        add_test(simple-tls-client-auth simple-tls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/truststore_client.p12)

        # add_test (simple-tls-sni-client-auth simple-tls-sni --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/keystore_server_cert.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/truststore_client.p12)
        add_test(simple-tls-client-auth simple-tls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/truststore_client.p12 --protocol_version 2.8)

        # add_test (simple-tls-sni-client-auth simple-tls-sni --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/keystore_server_cert.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/truststore_client.p12 --protocol_version 2.8)
        add_test(simple-tls-client-auth-mediatype simple-tls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/truststore_client.pem --protocol_version 2.8)
        add_test(stop_ssl_server_client_auth ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py stop)
        add_test(start_sasl_ssl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_ISPN_HOME} infinispan-sasl-ssl.xml)
        add_test(probe_sasl_ssl_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60)
        add_test(simpleSaslTls simpleSaslTls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/truststore_client.p12)
        add_test(stop_sasl_ssl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py stop)
        set_tests_properties(simple-tls simple-tls-mediatype simple-tls-client-auth simple-tls-client-auth-mediatype simpleSaslTls PROPERTIES WILL_FAIL TRUE)
    else(WIN32)
        # add_test (simple-tls-client-auth simple-tls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/truststore_client.pem)
        # add_test (simple-tls-sni-client-auth simple-tls-sni --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/keystore_server_cert.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/truststore_client.pem)
        # add_test (simple-tls-client-auth-mediatype simple-tls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/truststore_client.pem --protocol_version 2.8)
        # add_test (simple-tls-sni-client-auth-mediatype simple-tls-sni --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/keystore_server_cert.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/truststore_client.pem --protocol_version 2.8)
        add_test(stop_ssl_server_client_auth ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py stop)
        add_test(start_sasl_ssl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py start ${Java_JAVA_EXECUTABLE} ${HOTROD_ISPN_HOME} infinispan-sasl-ssl.xml)

        if(OPENSSL_VERSION VERSION_LESS "1.1.1")
            set_tests_properties(start_ssl_server PROPERTIES
                ENVIRONMENT JAVA_OPTS="-Dtest.useTLS=TLSv1.2")
        else(OPENSSL_VERSION VERSION_LESS "1.1.1")
            set_tests_properties(start_ssl_server PROPERTIES
                ENVIRONMENT JAVA_OPTS="-Dtest.useTLS=TLSv1.3")
        endif(OPENSSL_VERSION VERSION_LESS "1.1.1")

        add_test(probe_sasl_ssl_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60)
        add_test(simpleSaslTls simpleSaslTls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/truststore_client.pem)
        add_test(simpleSaslTls-mediatype simpleSaslTls --server_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/infinispan-ca.pem --client_cert_file ${CMAKE_CURRENT_SOURCE_DIR}/data/truststore_client.pem --protocol_version 2.8)
        add_test(stop_sasl_ssl_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin/server_ctl.py stop)
    endif(WIN32)
endif(NOT((EXISTS "${HOTROD_ISPN_HOME}/bin/server.sh") AND(EXISTS "${HOTROD_ISPN_HOME}/bin/server.bat")))
