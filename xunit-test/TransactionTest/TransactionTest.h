#include "gtest/gtest.h"
#include <memory>

namespace infinispan {
namespace hotrod
{
class RemoteCacheManager;
}
}

// The fixture for testing class Foo.

class TransactionTest : public ::testing::Test {
public:
	static std::unique_ptr<infinispan::hotrod::RemoteCacheManager> remoteCacheManager;
    static std::unique_ptr<infinispan::hotrod::RemoteCacheManager>nontxRemoteCacheManager;

protected:

    // You can do set-up work for each test here.
    TransactionTest();

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~TransactionTest() {} ;

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

};
