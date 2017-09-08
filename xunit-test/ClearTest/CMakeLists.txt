file(GLOB SRCS *.cpp)

add_executable(ClearTest ${SRCS})

add_definitions(-DHR_PROTO_EXPORT=)

target_link_libraries(ClearTest
    pthread
    libgtest
    libgmock
    hotrod
)

add_test(xunit_ClearTest ClearTest)


