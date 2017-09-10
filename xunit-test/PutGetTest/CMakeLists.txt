file(GLOB SRCS *.cpp)

add_executable(PutGetTest ${SRCS})

add_definitions(-DHR_PROTO_EXPORT=)
set_property(TARGET PutGetTest PROPERTY CXX_STANDARD 11)
set_property(TARGET PutGetTest PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties (PutGetTest PROPERTIES COMPILE_FLAGS "${COMPILER_FLAGS} ${WARNING_FLAGS_NO_PEDANTIC} ${STATIC_FLAGS} ${NO_UNUSED_FLAGS}")
set_target_properties(PutGetTest PROPERTIES COMPILE_DEFINITIONS "${DLLEXPORT_STATIC}" )

target_link_libraries(PutGetTest
    hotrod-static
    libgtest
    libgmock
    ${platform_libs}
)

add_test(xunit_PutGetTest PutGetTest)

