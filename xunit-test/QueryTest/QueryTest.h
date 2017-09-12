#include "gtest/gtest.h"
#include "xunit-test/QueryTest/bank-xunit.pb.h"
#include <memory>
#include <string>

namespace infinispan {
namespace hotrod
{
class RemoteCacheManager;
}
}

// The fixture for testing class Foo.

class QueryTest : public ::testing::Test {
public:
    static std::unique_ptr<infinispan::hotrod::RemoteCacheManager> remoteCacheManager;
    static const std::string ERRORS_KEY_SUFFIX;
    static const std::string PROTOBUF_METADATA_CACHE_NAME;
    static const std::string NAMED_CACHE;


protected:

    // You can do set-up work for each test here.
    QueryTest();

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~QueryTest() {} ;

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

};
