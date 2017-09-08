file(GLOB SRCS *.cpp)

add_executable(PutGetTest ${SRCS})

add_definitions(-DHR_PROTO_EXPORT=)

target_link_libraries(PutGetTest
    pthread
    libgtest
    libgmock
    hotrod
)

add_test(xunit_PutGetTest PutGetTest)

