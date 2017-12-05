file(GLOB SRCS *.cpp)


file(COPY ${CMAKE_ROOT_SOURCE_DIR}/test/query_proto/bank-xunit.proto DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

hr_protobuf_generate_cpp(TEST_PROTO_SRCS TEST_PROTO_HDRS .
  ${CMAKE_ROOT_SOURCE_DIR}/test/query_proto/bank-xunit.proto
)

add_executable(QueryTest ${SRCS} ${TEST_PROTO_SRCS})

add_definitions(-DHR_PROTO_EXPORT=)
set_property(TARGET QueryTest PROPERTY CXX_STANDARD 11)
set_property(TARGET QueryTest PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties (QueryTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${STATIC_FLAGS} ${NO_UNUSED_FLAGS}")
set_target_properties(QueryTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}" )

target_link_libraries(QueryTest
    hotrod-static
    libgtest
    ${platform_libs}
)

add_test (start_server ${PYTHON_EXECUTABLE} ${CMAKE_ROOT_SOURCE_DIR}/test/bin/server_ctl.py start ${JAVA_RUNTIME} ${HOTROD_JBOSS_HOME} clustered-indexing.xml)
add_test (probe_port ${PYTHON_EXECUTABLE} ${CMAKE_ROOT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60)

add_test(xunit_QueryTest QueryTest --gtest_output=xml:${PROJECT_BINARY_DIR}/test-output/)

add_test (stop_server ${PYTHON_EXECUTABLE} ${CMAKE_ROOT_SOURCE_DIR}/test/bin/server_ctl.py stop)
add_test (probe_port_stop ${PYTHON_EXECUTABLE} ${CMAKE_ROOT_SOURCE_DIR}/test/bin/probe_port.py localhost 11222 60 down)

