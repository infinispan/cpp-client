file(GLOB SRCS *.cpp)

add_executable(ClearTest ${SRCS})

add_definitions(-DHR_PROTO_EXPORT=)
set_property(TARGET ClearTest PROPERTY CXX_STANDARD 11)
set_property(TARGET ClearTest PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties (ClearTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${NO_UNUSED_FLAGS}")
set_target_properties(ClearTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}" )
target_link_libraries(ClearTest hotrod hotrod_protobuf ${PROTOBUF_LIBRARY} ${OPENSSL_LIBRARIES} gtest_main ${platform_libs})

add_test (start_server ${PYTHON_EXECUTABLE} ${CMAKE_ROOT_SOURCE_DIR}/bin/server_ctl.py start ${JAVA_RUNTIME} ${HOTROD_ISPN_HOME} infinispan.xml)
add_test (probe_port ${PYTHON_EXECUTABLE} ${CMAKE_ROOT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60)

add_test(xunit_ClearTest ClearTest --gtest_output=xml:${PROJECT_BINARY_DIR}/test-output/)

add_test (stop_server ${PYTHON_EXECUTABLE} ${CMAKE_ROOT_SOURCE_DIR}/bin/server_ctl.py stop)
add_test (probe_port_stop ${PYTHON_EXECUTABLE} ${CMAKE_ROOT_SOURCE_DIR}/bin/probe_port.py localhost 11222 60 down)

