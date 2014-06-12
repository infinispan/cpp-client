cmake_minimum_required (VERSION 2.6)

project (infinispan-hotrod-cpp C CXX)

set (CPACK_PACKAGE_VERSION_MAJOR "6")
set (CPACK_PACKAGE_VERSION_MINOR "1")
set (CPACK_PACKAGE_VERSION_PATCH "0-SNAPSHOT")
set (HOTROD_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  message (FATAL_ERROR "You are calling cmake from the source directory. Please create a separate build directory and call cmake from there. See README.md for details.")
endif ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src")

# MacOS has a bunch of differences in build tools and process and so we have to turn some things
# off there by default (for GCC; LLVM will need flags re-evaluated)
if (APPLE)
  set (NOENABLE_WARNING_ERROR ON)
  set (NOENABLE_UNDEFINED_ERROR ON)
  set (NOENABLE_SWIG_TESTING ON)
endif (APPLE)

if(WIN32 AND NOT CYGWIN)
  # Encourage use of valgrind unless unavailable on the platform
  set (NOENABLE_VALGRIND ON)
endif(WIN32 AND NOT CYGWIN)

# Add options here called <whatever> they will turn into "ENABLE_<whatever" and can be
# overridden on a platform specific basis above by NOENABLE_<whatever>
set (OPTIONS WARNING_ERROR UNDEFINED_ERROR SWIG_TESTING INTERNAL_TESTING VALGRIND)

foreach (OPTION ${OPTIONS})
  if (NOT "NOENABLE_${OPTION}")
    set ("DEFAULT_${OPTION}" ON)
  endif (NOT "NOENABLE_${OPTION}")
endforeach (OPTION)

# And add the option here too with help text
option(ENABLE_WARNING_ERROR "Consider compiler warnings to be errors" ${DEFAULT_WARNING_ERROR})
option(ENABLE_UNDEFINED_ERROR "Check for unresolved library symbols" ${DEFAULT_UNDEFINED_ERROR})
option(ENABLE_SWIG_TESTING "Create SWIG Java binding and test structure" ${DEFAULT_SWIG_TESTING})
option(ENABLE_INTERNAL_TESTING "Compile the library with internal tests (unit tests)" ${DEFAULT_INTERNAL_TESTING})
option(ENABLE_VALGRIND "Enable running the tests using Valgrind" ${DEFAULT_VALGRIND})

if (CMAKE_COMPILER_IS_GNUCXX)
  if (ENABLE_WARNING_ERROR)
    set (WERROR "-Werror")
  endif (ENABLE_WARNING_ERROR)

  set (COMPILER_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")
  set (WARNING_FLAGS
        "${WERROR} -pedantic -Wall -Wextra -Wno-shadow -Wpointer-arith -Wcast-qual -Wcast-align -Wno-long-long -Wvolatile-register-var -Winvalid-pch -Wno-system-headers -Woverloaded-virtual -Wshadow -Wno-variadic-macros")
  if (ENABLE_UNDEFINED_ERROR)
    set (CATCH_UNDEFINED "-Wl,--no-undefined")
  endif (ENABLE_UNDEFINED_ERROR)

endif (CMAKE_COMPILER_IS_GNUCXX)

if (MSVC)
   set (COMPILER_FLAGS "/DNOMINMAX /EHsc")
   set (WARNING_FLAGS "")
endif (MSVC)

if (NOT DEFINED COMPILER_FLAGS)
    message(FATAL_ERROR "Compiler flags not set for this build type")
endif (NOT DEFINED COMPILER_FLAGS)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS} ${WARNING_FLAGS}")

if(WIN32 AND NOT CYGWIN)
  # linking against Windows native libraries, including mingw
  set (HOTROD_WINAPI TRUE)
  set (platform_libs ws2_32 Rpcrt4)
elseif (APPLE)
  # OSX does not need rt
  set (platform_libs pthread)
else (WIN32 AND NOT CYGWIN)
  set (platform_libs pthread rt)
endif(WIN32 AND NOT CYGWIN)

if (ENABLE_VALGRIND)
  find_program(VALGRIND valgrind)
  if (VALGRIND MATCHES .*-NOTFOUND)
    message(FATAL_ERROR "Cannot find valgrind in your environment.  Please install valgrind or use NOENABLE_VALGRIND")
  endif(VALGRIND MATCHES .*-NOTFOUND)
  set(MEMORYCHECK_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/valgrind CACHE FILEPATH
      "Hot Rod Valgrind preprocessor" FORCE)
  set(MEMORYCHECK_COMMAND_OPTIONS
      "${VALGRIND} --leak-check=full --num-callers=25 --workaround-gcc296-bugs=yes --demangle=yes"
      CACHE STRING "Valgrind options" FORCE)
  configure_file ("${PROJECT_SOURCE_DIR}/CTestCustom.cmake" "${PROJECT_BINARY_DIR}/CTestCustom.cmake" COPYONLY)
endif (ENABLE_VALGRIND)

if (ENABLE_INTERNAL_TESTING)
set (internal_test_sources
  src/hotrod/test/Unit.cpp
  src/hotrod/test/L3Test.cpp
  src/hotrod/test/HashTest.cpp
)
endif (ENABLE_INTERNAL_TESTING)


# Select driver
if(HOTROD_WINAPI)
  set (platform_sources src/hotrod/sys/windows/Socket.cpp src/hotrod/sys/windows/Thread.cpp
           src/hotrod/sys/windows/platform.cpp src/hotrod/sys/windows/Inet.cpp src/hotrod/sys/windows/Time.cpp)
else(HOTROD_WINAPI)
  set (platform_sources src/hotrod/sys/posix/Socket.cpp src/hotrod/sys/posix/Thread.cpp
           src/hotrod/sys/posix/platform.cpp src/hotrod/sys/posix/Mutex.cpp src/hotrod/sys/posix/Inet.cpp src/hotrod/sys/posix/Time.cpp)
endif(HOTROD_WINAPI)

configure_file(src/hotrod/impl/Version.cpp.in ${CMAKE_BINARY_DIR}/Version.cpp @ONLY)

set (library_sources
  src/hotrod/api/RemoteCacheManager.cpp
  src/hotrod/api/RemoteCacheBase.cpp
  src/hotrod/api/exceptions.cpp
  src/hotrod/impl/configuration/Configuration.cpp
  src/hotrod/impl/configuration/ConfigurationBuilder.cpp
  src/hotrod/impl/configuration/ConnectionPoolConfiguration.cpp
  src/hotrod/impl/RemoteCacheManagerImpl.cpp
  src/hotrod/impl/RemoteCacheImpl.cpp
  src/hotrod/impl/IntWrapper.cpp
  src/hotrod/impl/hash/MurmurHash2.cpp
  src/hotrod/impl/hash/MurmurHash3.cpp
  src/hotrod/impl/consistenthash/ConsistentHashFactory.cpp
  src/hotrod/impl/consistenthash/ConsistentHashV1.cpp
  src/hotrod/impl/consistenthash/ConsistentHashV2.cpp
  src/hotrod/impl/operations/OperationsFactory.cpp
  src/hotrod/impl/operations/PingOperation.cpp
  src/hotrod/impl/operations/GetOperation.cpp
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
  src/hotrod/impl/operations/FaultTolerantPingOperation.cpp
  src/hotrod/impl/protocol/HeaderParams.cpp
  src/hotrod/impl/protocol/Codec10.cpp
  src/hotrod/impl/protocol/Codec11.cpp
  src/hotrod/impl/protocol/Codec12.cpp
  src/hotrod/impl/protocol/CodecFactory.cpp
  src/hotrod/impl/transport/AbstractTransport.cpp
  src/hotrod/impl/transport/tcp/ConnectionPool.cpp
  src/hotrod/impl/transport/tcp/InetSocketAddress.cpp
  src/hotrod/impl/transport/tcp/Socket.cpp
  src/hotrod/impl/transport/tcp/TcpTransport.cpp
  src/hotrod/impl/transport/tcp/TcpTransportFactory.cpp
  src/hotrod/impl/transport/tcp/TransportObjectFactory.cpp
  src/hotrod/impl/transport/tcp/RoundRobinBalancingStrategy.cpp
  src/hotrod/sys/Runnable.cpp
  src/hotrod/sys/Log.cpp
  ${platform_sources}
  ${CMAKE_BINARY_DIR}/Version.cpp
  ${internal_test_sources}  
)

# Build a shared library
add_library (hotrod SHARED ${library_sources})
target_link_libraries (hotrod ${platform_libs})

if (WIN32 AND NOT CMAKE_SIZEOF_VOID_P MATCHES "8")
    set (OUTPUT_NAME_SUFFIX "32")
else (WIN32 AND NOT CMAKE_SIZEOF_VOID_P MATCHES "8")
    set (OUTPUT_NAME_SUFFIX "")
endif (WIN32 AND NOT CMAKE_SIZEOF_VOID_P MATCHES "8")

set_target_properties (hotrod PROPERTIES OUTPUT_NAME "hotrod${OUTPUT_NAME_SUFFIX}")
set_target_properties (hotrod PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS}")
set_target_properties (hotrod PROPERTIES LINK_FLAGS "${CATCH_UNDEFINED}")
set_target_properties (hotrod PROPERTIES SOVERSION "1.0")

# Build a static library
add_library (hotrod-static STATIC ${library_sources})
set_target_properties(hotrod-static PROPERTIES COMPILE_FLAGS "-DHOTROD_DECLARE_STATIC ${COMPILER_FLAGS} ${WARNING_FLAGS}")

# TESTS

add_executable (simple test/Simple.cpp)
set_property(TARGET simple PROPERTY INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_target_properties (simple PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS}")
target_link_libraries (simple hotrod)

if (ENABLE_INTERNAL_TESTING)
    add_executable (unit_test test/Unit.cpp)
    set_target_properties (unit_test PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS}")
    target_link_libraries (unit_test hotrod)
endif (ENABLE_INTERNAL_TESTING)

add_executable (itest test/InteractiveTest.cpp)
set_target_properties (itest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS}")
target_link_libraries (itest hotrod)

# the CTest include must be after the MEMORYCHECK settings are processed
include (CTest)

if (ENABLE_INTERNAL_TESTING)
    add_test (unit_test unit_test)
endif (ENABLE_INTERNAL_TESTING)    

find_package(Java)

if (NOT DEFINED HOTROD_JBOSS_HOME)
   if (NOT DEFINED ENV{JBOSS_HOME})
      message(WARNING "you must set the JBOSS_HOME environment variable or use -DHOTROD_JBOSS_HOME=/the/path if you want to run integration tests")
   else (NOT DEFINED ENV{JBOSS_HOME})
      set(HOTROD_JBOSS_HOME $ENV{JBOSS_HOME} CACHE FILEPATH "Infinispan HOME dir")
   endif (NOT DEFINED ENV{JBOSS_HOME})
endif (NOT DEFINED HOTROD_JBOSS_HOME)

if (NOT ((EXISTS "${HOTROD_JBOSS_HOME}/bin/standalone.sh") AND (EXISTS "${HOTROD_JBOSS_HOME}/bin/standalone.bat")))
    message(WARNING "JBOSS_HOME ${HOTROD_JBOSS_HOME} does not have needed startup scripts")
else (NOT ((EXISTS "${HOTROD_JBOSS_HOME}/bin/standalone.sh") AND (EXISTS "${HOTROD_JBOSS_HOME}/bin/standalone.bat")))
    if (NOT JAVA_RUNTIME)
        message(FATAL_ERROR "Java not found")
    endif (NOT JAVA_RUNTIME)
    find_package(PythonInterp REQUIRED)
    add_test (start_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py start ${JAVA_RUNTIME} ${HOTROD_JBOSS_HOME} single)
    add_test (probe_port ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60)
    add_test (simple simple)
    add_test (stop_server ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/bin/server_ctl.py stop)
endif (NOT ((EXISTS "${HOTROD_JBOSS_HOME}/bin/standalone.sh") AND (EXISTS "${HOTROD_JBOSS_HOME}/bin/standalone.bat")))

if (ENABLE_SWIG_TESTING)
    if (NOT JAVA_RUNTIME)
        message(WARNING "Cannot enable SWIG tests without Java")
    else (NOT JAVA_RUNTIME)
        include(jni/swig.cmake)
    endif (NOT JAVA_RUNTIME)
endif (ENABLE_SWIG_TESTING)

if (ENABLE_VALGRIND)
    add_custom_target(memtest ${CMAKE_CTEST_COMMAND} -D ExperimentalMemCheck
        COMMENT "Running CTest with Valgrind options")

    add_custom_target(memtestv ${CMAKE_CTEST_COMMAND} -V -D ExperimentalMemCheck
        COMMENT "Running CTest in verbose mode with Valgrind options")
endif (ENABLE_VALGRIND)

set (CPACK_GENERATOR "ZIP")
set (CPACK_SOURCE_GENERATOR "ZIP")
set (CPACK_INCLUDE_TOPLEVEL_DIRECTORY "1")
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

file (GLOB includes "${CMAKE_CURRENT_SOURCE_DIR}/include/infinispan/hotrod/*.h")
install (FILES ${includes} DESTINATION include/infinispan/hotrod)
install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/License.txt" "${CMAKE_CURRENT_SOURCE_DIR}/dist/README.md" DESTINATION ${DOCDIR})
install (TARGETS hotrod hotrod-static DESTINATION lib${LIB_SUFFIX})

include (CPack)

# API DOCS -- use target docs (i.e. make docs) from build directory to make api docs
# Toggle BUILD_DOCS to turn on/off the generation of the API documentation
FIND_PACKAGE(Doxygen)
if (DOXYGEN_FOUND)
  #-- Configure the Template Doxyfile for our specific project
  configure_file(Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
  # Delete any previously generated docs
  if(EXISTS ${CMAKE_BINARY_DIR}/api_docs)
      file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/api_docs)
  endif(EXISTS ${CMAKE_BINARY_DIR}/api_docs)
  #-- Add a custom target to run Doxygen when ever the project is built
  add_custom_target (docs ALL
                     COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
                     SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
  #Include the API docs in the package.
  install (FILES ${CMAKE_BINARY_DIR}/api_docs/html/ DESTINATION ${DOCDIR}/api)
endif (DOXYGEN_FOUND)

