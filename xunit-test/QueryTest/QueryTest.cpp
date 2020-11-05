#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include <infinispan/hotrod/BasicTypesProtoStreamMarshaller.h>
#include <infinispan/hotrod/ProtoStreamMarshaller.h>
#include "QueryTest.h"
#include "infinispan/hotrod/query.pb.h"
#include "infinispan/hotrod/QueryUtils.h"

#include <cstdio>
using ::infinispan::hotrod::ConfigurationBuilder;
using ::infinispan::hotrod::Configuration;

std::unique_ptr<infinispan::hotrod::RemoteCacheManager> QueryTest::remoteCacheManager;
const std::string QueryTest::ERRORS_KEY_SUFFIX = ".errors";
const std::string QueryTest::PROTOBUF_METADATA_CACHE_NAME = "___protobuf_metadata";
const std::string QueryTest::NAMED_CACHE = "InMemoryNonSharedIndex";

QueryTest::QueryTest()
{
}

std::string read(std::string file)
{
    std::ifstream t(file);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

void putUsers(RemoteCache<int, sample_bank_account::User> &remoteCache)
{
    sample_bank_account::User user1;
    user1.set_id(1);
    user1.set_name("John");
    user1.set_surname("Doe");
    user1.set_gender(sample_bank_account::User_Gender_MALE);
    user1.set_age(22);
    user1.set_notes("Lorem ipsum dolor sit amet");

    user1.add_accountids(1);
    user1.add_accountids(2);

    auto address1 = user1.add_addresses();
    address1->set_street("Main Street");
    address1->set_postcode("X1234");
    address1->set_number(156);

    remoteCache.put(1, user1);

    sample_bank_account::User user2;
    user2.set_id(2);
    user2.set_name("Spider");
    user2.set_surname("Man");
    user2.set_gender(sample_bank_account::User_Gender_MALE);
    //user2.set_age(23);

    user2.add_accountids(3);

    auto address2 = user2.add_addresses();
    address2->set_street("Old Street");
    address2->set_postcode("Y12");
    address2->set_number(-12);

    auto address3 = user2.add_addresses();
    address3->set_street("Bond Street");
    address3->set_postcode("ZZ");
    address3->set_number(312);

    remoteCache.put(2, user2);

    sample_bank_account::User user3;
    user3.set_id(3);
    user3.set_name("Spider");
    user3.set_surname("Woman");
    user3.set_gender(sample_bank_account::User_Gender_FEMALE);

    remoteCache.put(3, user3);
}

unsigned long int MakeDate(int year, int month, int day)
{
    time_t rawtime;
    struct tm * timeinfo;

    /* get current timeinfo: */
    time(&rawtime); //or: rawtime = time(0);
    /* convert to struct: */
    timeinfo = localtime(&rawtime);

    /* now modify the timeinfo to the given date: */
    timeinfo->tm_year = year - 1900;
    timeinfo->tm_mon = month - 1;    //months since January - [0,11]
    timeinfo->tm_mday = day;          //day of the month - [1,31]
    timeinfo->tm_hour = 0;         //hours since midnight - [0,23]
    timeinfo->tm_min = 0;          //minutes after the hour - [0,59]
    timeinfo->tm_sec = 0;          //seconds after the minute - [0,59]

    /* call mktime: create unix time stamp from timeinfo struct */
#if !defined _WIN32 && !defined _WIN64
    return timegm(timeinfo);
#else
    return _mkgmtime(timeinfo);
#endif
}

void PutAccounts(RemoteCache<int, sample_bank_account::Account>& remoteCache)
{
    sample_bank_account::Account account1;
    account1.set_id(1);
    account1.set_description("John Doe's first bank account");
    account1.set_creationdate(MakeDate(2013, 01, 03));

    remoteCache.put(4, account1);

    sample_bank_account::Account account2;
    account2.set_id(2);
    account2.set_description("John Doe's second bank account");
    account2.set_creationdate(MakeDate(2013, 01, 04));

    remoteCache.put(5, account2);

    sample_bank_account::Account account3;
    account3.set_id(3);
    account3.set_creationdate(MakeDate(2013, 01, 20));

    remoteCache.put(6, account3);
}

void PutTransactions(RemoteCache<int, sample_bank_account::Transaction>& remoteCache)
{
    sample_bank_account::Transaction transaction0;
    transaction0.set_id(0);
    transaction0.set_description("Birthday present");
    transaction0.set_accountid(1);
    transaction0.set_amount(1800);
    transaction0.set_date(MakeDate(2012,9,7));
    transaction0.set_isdebit(false);
    transaction0.set_isvalid(true);
    remoteCache.put(7, transaction0);

    sample_bank_account::Transaction transaction1;
    transaction1.set_id(1);
    transaction1.set_description("Feb. rent payment");
    transaction1.set_accountid(1);
    transaction1.set_amount(1500);
    transaction1.set_date(MakeDate(2013,1,5));
    transaction1.set_isdebit(true);
    transaction1.set_isvalid(true);
    remoteCache.put(8, transaction1);

    sample_bank_account::Transaction transaction2;
    transaction2.set_id(2);
    transaction2.set_description("Starbucks");
    transaction2.set_accountid(1);
    transaction2.set_amount(23);
    transaction2.set_date(MakeDate(2013,1,9));
    transaction2.set_isdebit(true);
    transaction2.set_isvalid(true);
    remoteCache.put(9, transaction2);

    sample_bank_account::Transaction transaction3;
    transaction3.set_id(3);
    transaction3.set_description("Hotel");
    transaction3.set_accountid(2);
    transaction3.set_amount(45);
    transaction3.set_date(MakeDate(2013,2,27));
    transaction3.set_isdebit(true);
    transaction3.set_isvalid(true);
    remoteCache.put(10, transaction3);

    sample_bank_account::Transaction transaction4;
    transaction4.set_id(4);
    transaction4.set_description("Last january");
    transaction4.set_accountid(2);
    transaction4.set_amount(95);
    transaction4.set_date(MakeDate(2013,1,31));
    transaction4.set_isdebit(true);
    transaction4.set_isvalid(true);
    remoteCache.put(11, transaction4);

    sample_bank_account::Transaction transaction5;
    transaction5.set_id(5);
    transaction5.set_description("-Popcorn");
    transaction5.set_accountid(2);
    transaction5.set_amount(4);
    transaction5.set_date(MakeDate(2013,1,01));
    transaction5.set_isdebit(true);
    transaction5.set_isvalid(true);
    remoteCache.put(12, transaction5);
}

void QueryTest::SetUp()
{
    if (QueryTest::remoteCacheManager == nullptr) {
        ConfigurationBuilder conf;
        conf.addServer().host("127.0.0.1").port(11222);
        conf.protocolVersion(Configuration::PROTOCOL_VERSION_24);
        conf.connectionTimeout(90000).socketTimeout(6000);
        conf.balancingStrategyProducer(nullptr);
        QueryTest::remoteCacheManager.reset(new RemoteCacheManager(conf.build(), false));
        //initialize server-side serialization
        auto *km = new BasicTypesProtoStreamMarshaller<std::string>();
        auto *vm = new BasicTypesProtoStreamMarshaller<std::string>();
        remoteCacheManager->start();
        RemoteCache<std::string, std::string> metadataCache = remoteCacheManager->getCache<std::string, std::string>(
                km, &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy,
                PROTOBUF_METADATA_CACHE_NAME, false);
        std::string protoDef(read("bank-xunit.proto"));
        if (protoDef.empty())
        {
            FAIL() << "Error in reading .proto file";
        }
        metadataCache.remove("sample_bank_account/bank-xunit.proto");
        metadataCache.put("sample_bank_account/bank-xunit.proto", protoDef);
        if (metadataCache.containsKey(ERRORS_KEY_SUFFIX))
        {
            FAIL()<< "fail: error in registering .proto model" << std::endl;
        }

        auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
        auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
        RemoteCache<int, sample_bank_account::User> userCache = remoteCacheManager->getCache<int,
                sample_bank_account::User>(
                testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy, NAMED_CACHE,
                false);
        userCache.clear();
        putUsers(userCache);

        auto *acc_testkm = new BasicTypesProtoStreamMarshaller<int>();
        auto *acc_testvm = new ProtoStreamMarshaller<sample_bank_account::Account, 1000043>();
        RemoteCache<int, sample_bank_account::Account> accCache = remoteCacheManager->getCache<int,
                sample_bank_account::Account>(
                acc_testkm, &Marshaller<int>::destroy, acc_testvm, &Marshaller<sample_bank_account::Account>::destroy,
                NAMED_CACHE,
                false);
        PutAccounts(accCache);

        auto *tra_testkm = new BasicTypesProtoStreamMarshaller<int>();
        auto *tra_testvm = new ProtoStreamMarshaller<sample_bank_account::Transaction, 1000044>();
        RemoteCache<int, sample_bank_account::Transaction> traCache = remoteCacheManager->getCache<int,
                sample_bank_account::Transaction>(
                tra_testkm, &Marshaller<int>::destroy, tra_testvm, &Marshaller<sample_bank_account::Transaction>::destroy,
                NAMED_CACHE,
                false);
        PutTransactions(traCache);


    }
}
;

void QueryTest::TearDown()
{
}

TEST_F(QueryTest, getAll)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.User");
    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    EXPECT_EQ(3, vectorOfUsers.size());
}

TEST_F(QueryTest, Eq1Test)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    // qr.JpqlString = "from sample_bank_account.User u where u.name = \"John\"";
    qr.set_querystring("from sample_bank_account.User u where u.name = \"John\"");
    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(1, vectorOfUsers.size());
    ASSERT_EQ("John", vectorOfUsers[0].name());
    ASSERT_EQ("Doe", vectorOfUsers[0].surname());
}

TEST_F(QueryTest, EqEmptyStringTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    // qr.JpqlString = "from sample_bank_account.User u where u.name = \"John\"";
    qr.set_querystring("from sample_bank_account.User u where u.name = \"\"");

    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(0, vectorOfUsers.size());
}

TEST_F(QueryTest, EqSentenceTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    // qr.JpqlString = "from sample_bank_account.Account a where a.description = \"John Doe's first bank account\"";
    qr.set_querystring("from sample_bank_account.Account a where a.description = \"John Doe's first bank account\"");

    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::Account> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(1, vectorOfUsers.size());
    ASSERT_EQ(1, vectorOfUsers[0].id());
}

TEST_F(QueryTest, EqNonIndexedFieldTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.User u where u.notes = \"Lorem ipsum dolor sit amet\"");

    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(1, vectorOfUsers.size());
    ASSERT_EQ(1, vectorOfUsers[0].id());
}

TEST_F(QueryTest, EqInNested1Test)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.User u where u.addresses.postCode = \"X1234\"");

    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(1, vectorOfUsers.size());
    ASSERT_EQ("X1234", vectorOfUsers[0].addresses(0).postcode());
}

TEST_F(QueryTest, LikeTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.Transaction t where t.description like \"%rent%\"");

    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::Transaction> vectorOfTransaction;
    if (!unwrapResults(result, vectorOfTransaction)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(1, vectorOfTransaction.size());
    ASSERT_EQ(1, vectorOfTransaction[0].accountid());
    ASSERT_EQ(1500, vectorOfTransaction[0].amount());
}

TEST_F(QueryTest, BetweenTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    char strbuf[512];
    std::sprintf(strbuf, "from sample_bank_account.Transaction t where t.date between \"%d\" and \"%d\"",MakeDate(2013,1,1), MakeDate(2013,1,31));
    qr.set_querystring(std::string(strbuf));

    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::Transaction> vectorOfTransaction;
    if (!unwrapResults(result, vectorOfTransaction)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(4, vectorOfTransaction.size());
    for(auto t : vectorOfTransaction)
    {
    ASSERT_LE(MakeDate(2013,01,01), t.date());
    ASSERT_GE(MakeDate(2013,31,01), t.date());
    }
}

TEST_F(QueryTest, GreatThanTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.Transaction t where t.amount > 1500");

    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::Transaction> vectorOfTransaction;
    if (!unwrapResults(result, vectorOfTransaction)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(1, vectorOfTransaction.size());
    ASSERT_LT(1500, vectorOfTransaction[0].amount());
}

TEST_F(QueryTest, OrTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.User u where (u.surname = \"Man\") or (u.surname = \"Woman\")");

    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(2, vectorOfUsers.size());
    for (auto u : vectorOfUsers)
    {
        ASSERT_EQ("Spider", vectorOfUsers[0].name());

    }
}

TEST_F(QueryTest, NotTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.User u where u.name != \"Spider\"");

    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(1, vectorOfUsers.size());
    ASSERT_EQ("John", vectorOfUsers[0].name());
}

TEST_F(QueryTest, InvalidEmbeddedAttributeTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("select u.addresses from sample_bank_account.User u");
    EXPECT_THROW(userCache.query(qr), HotRodClientException);
}

TEST_F(QueryTest, RejectProjectionOfRepeatedPropertyTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("select u.addresses.postcode from sample_bank_account.User u");
    EXPECT_THROW(userCache.query(qr), HotRodClientException);
}

// Response unwrapper for int*. Can't use int because result contains null values
template <> inline int* unwrapSingleValue<int*>(const WrappedMessage& wm)
{
    if (wm.has_wrappedint32())
    {
    return new int((int)wm.wrappedint32());
    }
    else if (wm.has_wrappedint64())
    {
        return new int((int)wm.wrappedint64());
    }
    else
    {
        return nullptr;
    }
}

template<>
inline std::tuple<int*> popTuple<int*>(QueryResponse& resp, int &k)
        {
      int* s(unwrapSingleValue<int*>(resp.results(k++)));
      return std::make_tuple<int*>(std::move(s));
}



TEST_F(QueryTest, ProjectionTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("select u.name, u.surname, u.age from sample_bank_account.User u where u.age is null");

    QueryResponse result = userCache.query(qr);

    std::vector<std::tuple<std::string, std::string, int* > > projections;
    if (!unwrapProjection(result, projections)) {
        FAIL()<< "fail: error in unwrapping projection"
        << std::endl;
    }

    ASSERT_EQ(2, projections.size());

	ASSERT_EQ("Spider", (std::get<0>(projections[0])));
	ASSERT_EQ("Man", (std::get<1>(projections[0])));
	ASSERT_EQ("Spider", (std::get<0>(projections[1])));
	ASSERT_EQ("Woman", (std::get<1>(projections[1])));
	for (auto &i: projections)
	{
			if(std::get<2>(i)!=nullptr)
		{
				delete std::get<2>(i);
		}
	}
}

TEST_F(QueryTest, ContainsTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.User u where u.accountIds = 2");
    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(1, vectorOfUsers.size());
    ASSERT_EQ("John", vectorOfUsers[0].name());
}

TEST_F(QueryTest, ContainsAllTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.User u where u.accountIds = 1 and u.accountIds = 2");
    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(1, vectorOfUsers.size());
    ASSERT_EQ(1, vectorOfUsers[0].id());
}

TEST_F(QueryTest, NotContainsAllTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.User u where u.accountIds = 1 and u.accountIds = 2 and u.accountIds = 3");
    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(0, vectorOfUsers.size());
}

TEST_F(QueryTest, NotContainsTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.User u where u.accountIds = 1000042");
    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(0, vectorOfUsers.size());
}

TEST_F(QueryTest, ContainsAnyTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.User u where u.accountIds = 4 or u.accountIds = 5");
    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(0, vectorOfUsers.size());
}

TEST_F(QueryTest, InTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.User u where u.id in (1, 3)");
    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(2, vectorOfUsers.size());
}

TEST_F(QueryTest, NotInTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.User u where u.id in (4)");
    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::User> vectorOfUsers;
    if (!unwrapResults(result, vectorOfUsers)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(0, vectorOfUsers.size());
}

TEST_F(QueryTest, StringLiteralEscapeTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("from sample_bank_account.Account a where a.description = 'John Doe''s first bank account'");

    QueryResponse result = userCache.query(qr);
    std::vector<sample_bank_account::Account> vectorOfAccounts;
    if (!unwrapResults(result, vectorOfAccounts)) {
        FAIL()<< "fail: found unexpected projection in resultset"
        << std::endl;
    }
    ASSERT_EQ(1, vectorOfAccounts.size());
    ASSERT_EQ(1, vectorOfAccounts[0].id());
}

TEST_F(QueryTest, HavingWithSumTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("select t.accountId, sum(t.amount) from sample_bank_account.Transaction t group by t.accountId having sum(t.amount) > 3300");

    QueryResponse result = userCache.query(qr);

    std::vector<std::tuple<google::protobuf::int32, double> > projections;
    if (!unwrapProjection(result, projections)) {
        FAIL()<< "fail: error in unwrapping projection"
        << std::endl;
    }

    ASSERT_EQ(1, projections.size());

    ASSERT_EQ(1, (std::get<0>(projections[0])));
    ASSERT_EQ(3323, (std::get<1>(projections[0])));
}

TEST_F(QueryTest, HavingWithAvgTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("select t.accountId, avg(t.amount) from sample_bank_account.Transaction t group by t.accountId having avg(t.amount) < 100");

    QueryResponse result = userCache.query(qr);

    std::vector<std::tuple<google::protobuf::int32, double> > projections;
    if (!unwrapProjection(result, projections)) {
        FAIL()<< "fail: error in unwrapping projection"
        << std::endl;
    }

    ASSERT_EQ(1, projections.size());

    ASSERT_EQ(2, (std::get<0>(projections[0])));
    ASSERT_EQ(48, (std::get<1>(projections[0])));
}

TEST_F(QueryTest, HavingWithMinTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("select t.accountId, min(t.amount) from sample_bank_account.Transaction t group by t.accountId having min(t.amount) < 10");

    QueryResponse result = userCache.query(qr);

    std::vector<std::tuple<google::protobuf::int32, double> > projections;
    if (!unwrapProjection(result, projections)) {
        FAIL()<< "fail: error in unwrapping projection"
        << std::endl;
    }

    ASSERT_EQ(1, projections.size());

    ASSERT_EQ(2, (std::get<0>(projections[0])));
    ASSERT_EQ(4, (std::get<1>(projections[0])));
}

TEST_F(QueryTest, HavingWithMaxTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("select t.accountId, max(t.amount) from sample_bank_account.Transaction t group by t.accountId having max(t.amount) > 1000");

    QueryResponse result = userCache.query(qr);

    std::vector<std::tuple<google::protobuf::int32, double> > projections;
    if (!unwrapProjection(result, projections)) {
        FAIL()<< "fail: error in unwrapping projection"
        << std::endl;
    }

    ASSERT_EQ(1, projections.size());

    ASSERT_EQ(1, (std::get<0>(projections[0])));
    ASSERT_EQ(1800, (std::get<1>(projections[0])));
}

TEST_F(QueryTest, GlobalTest)
{
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> userCache =
            remoteCacheManager->getCache<int, sample_bank_account::User>(
                    testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy,
                    NAMED_CACHE, false);
    QueryRequest qr;
    // JpqlString will be deprecated please use QueryString
    qr.set_querystring("select sum(t.amount) from sample_bank_account.Transaction t");

    QueryResponse result = userCache.query(qr);

    std::vector<std::tuple<double> > projections;
    if (!unwrapProjection(result, projections)) {
        FAIL()<< "fail: error in unwrapping projection"
        << std::endl;
    }

    ASSERT_EQ(1, projections.size());

    ASSERT_EQ(3467, (std::get<0>(projections[0])));
}

