/*
 * QueryTest.cpp
 *
 *  Created on: Apr 6, 2016
 *      Author: rigazilla
 */
#include <infinispan/hotrod/BasicTypesProtoStreamMarshaller.h>
#include "addressbook.pb.h"
#include "bank.pb.h"
#include <infinispan/hotrod/ProtoStreamMarshaller.h>
#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/Version.h"
#include "infinispan/hotrod/Query.h"
#include "infinispan/hotrod/QueryUtils.h"
#include "infinispan/hotrod/CacheClientListener.h"
#include "infinispan/hotrod/ContinuousQueryListener.h"

#include "infinispan/hotrod/JBasicMarshaller.h"
#include <vector>
#include <tuple>

#define PROTOBUF_METADATA_CACHE_NAME "___protobuf_metadata"
#define ERRORS_KEY_SUFFIX  ".errors"

using namespace infinispan::hotrod;
using namespace org::infinispan::query::remote::client;

std::string read(std::string file)
		{
	std::ifstream t(file);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

class ResourceManager{
private:
	std::vector<std::function<void()> > cleanups;
public:
	~ResourceManager() {
        while (cleanups.size() > 0)
        {
            cleanups.back()();
            cleanups.pop_back();
        }
	}
	void add(std::function<void()> cleanup)
	{
		cleanups.push_back(cleanup);
	}
};

int main(int argc, char** argv) {
	int result = 0;
	std::cout << "Tests for Continuous Query" << std::endl;
	ConfigurationBuilder builder;
	builder.addServer().host(argc > 1 ? argv[1] : "127.0.0.1").port(11222);
	builder.protocolVersion(Configuration::PROTOCOL_VERSION_26);
	builder.balancingStrategyProducer(nullptr);
	RemoteCacheManager cacheManager(builder.build(), false);

    //initialize server-side serialization
	auto *km = new BasicTypesProtoStreamMarshaller<std::string>();
	auto *vm = new BasicTypesProtoStreamMarshaller<std::string>();

	RemoteCache<std::string, std::string> metadataCache = cacheManager.getCache<std::string, std::string>(
			km, &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy, PROTOBUF_METADATA_CACHE_NAME,
			false);

    ResourceManager rMain;
    cacheManager.start();

    rMain.add([&cacheManager] { cacheManager.stop(); });

    metadataCache.put("sample_bank_account/bank.proto", read("query_proto/bank.proto"));
	if (metadataCache.containsKey(ERRORS_KEY_SUFFIX))
			{
		std::cerr << "fail: error in registering .proto model" << std::endl;
		result = -1;
		return result;
	}
    rMain.add([&metadataCache] {
        metadataCache.remove("sample_bank_account/bank.proto");
    });
    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
	auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
	RemoteCache<int, sample_bank_account::User> testCache = cacheManager.getCache<int, sample_bank_account::User>(
			testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy, false);
	ContinuousQueryListener<int, sample_bank_account::User> cql(testCache,
			"from sample_bank_account.User");
	{
		ResourceManager r;
		std::cout << "Testing query: from sample_bank_account.User" << std::endl;

		testCache.clear();

		std::promise<void> promise;
		int createdCount = 0, changedCount = 0, removedCount = 0;
		std::function<void(int, sample_bank_account::User)> join =
				[&promise, &createdCount](int k, sample_bank_account::User u)
				{
					std::cout << "JOINING: key="<< u.id() << " value="<< u.name() << std::endl;
					++createdCount;
				};

		std::function<void(int, sample_bank_account::User)> leave =
				[&promise, &removedCount](int k, sample_bank_account::User u)
				{
					std::cout << "LEAVING: key="<< u.id() << " value="<< u.name() << std::endl;
					++removedCount;
					promise.set_value();
				};

		std::function<void(int, sample_bank_account::User)> change =
				[&promise, &changedCount](int k, sample_bank_account::User u)
				{
					std::cout << "CHANGING: key="<< u.id() << " value="<< u.name() << std::endl;
					++changedCount;
				};

		cql.setJoiningListener(join);
		cql.setLeavingListener(leave);
		cql.setUpdatedListener(change);
		testCache.addContinuousQueryListener(cql);
        r.add([&testCache,&cql](){ testCache.removeContinuousQueryListener(cql);});
		sample_bank_account::User_Address a;
		sample_bank_account::User user1;
		user1.set_id(1);
		user1.set_name("Tom");
		user1.set_surname("Cat");
		user1.set_gender(sample_bank_account::User_Gender_MALE);
		sample_bank_account::User_Address * addr = user1.add_addresses();
		addr->set_street("Via Roma");
		addr->set_number(3);
		addr->set_postcode("202020");
		testCache.put(1, user1);

		user1.set_id(2);
		user1.set_name("Jerry");
		user1.set_surname("Mouse");
		user1.set_gender(sample_bank_account::User_Gender_MALE);
		testCache.put(2, user1);

		user1.set_id(2);
		user1.set_name("Mickey");
		testCache.put(2, user1);

		testCache.remove(2);

		if (std::future_status::timeout
				== promise.get_future().wait_for(std::chrono::seconds(30))) {
			std::cout << "FAIL: Continuous query events (Timeout)" << std::endl;
			return -1;
		}

		if (createdCount != 2 || changedCount != 1 || removedCount != 1) {
			std::cout
			<< "FAIL: (createdCount,changedCount,removedCount) is  ("
					<< createdCount << ", " << changedCount << ", "
					<< removedCount << ")" << "  should be (2,1,1)"
					<< std::endl;
			return -1;
		}
	}
	
	ContinuousQueryListener<int, sample_bank_account::User, int, std::string> cql1(
			testCache, "select id, name from sample_bank_account.User");
	{
		ResourceManager r;
		std::cout << "Testing query: select id, name from sample_bank_account.User" << std::endl;
		testCache.clear();

		std::promise<void> promise;
		std::promise<void> promise_where;
		int createdCount = 0, changedCount = 0, removedCount = 0;

		std::function<void(int, std::tuple<int, std::string>)> join =
				[&promise, &createdCount](int k, std::tuple<int, std::string> t)
				{
					std::cout << "JOINING: key="<< std::get<0>(t) << " value="<< std::get<1>(t) << std::endl;
					++createdCount;
				};

		std::function<void(int, std::tuple<int, std::string>)> leave =
				[&promise, &removedCount](int k, std::tuple<int, std::string> t)
				{
					std::cout << "LEAVING: key="<< std::get<0>(t) << " value="<< std::get<1>(t) << std::endl;
					++removedCount;
					promise.set_value();
				};

		std::function<void(int, std::tuple<int, std::string>)> change =
				[&promise, &changedCount](int k, std::tuple<int, std::string> t)
				{
					std::cout << "CHANGING: key="<< std::get<0>(t) << " value="<< std::get<1>(t) << std::endl;
					++changedCount;
				};

		cql1.setJoiningListener(join);
		cql1.setLeavingListener(leave);
		cql1.setUpdatedListener(change);

		std::vector<char> param;

		std::string qString("select id, name from sample_bank_account.User");

		BasicTypesProtoStreamMarshaller<std::string> paramMarshaller;

		std::vector<std::vector<char>> filterFactoryParams;
		paramMarshaller.marshall(qString, param);
		filterFactoryParams.push_back(param);
		std::vector<std::vector<char> > converterFactoryParams;
		char CONTINUOUS_QUERY_FILTER_FACTORY_NAME[] =
				"continuous-query-filter-converter-factory";
		CacheClientListener<int, sample_bank_account::User> cl(testCache);
		cl.filterFactoryName = std::vector<char>(
				CONTINUOUS_QUERY_FILTER_FACTORY_NAME,
				CONTINUOUS_QUERY_FILTER_FACTORY_NAME
						+ strlen(CONTINUOUS_QUERY_FILTER_FACTORY_NAME));
		cl.converterFactoryName = std::vector<char>(
				CONTINUOUS_QUERY_FILTER_FACTORY_NAME,
				CONTINUOUS_QUERY_FILTER_FACTORY_NAME
						+ strlen(CONTINUOUS_QUERY_FILTER_FACTORY_NAME));

		testCache.addContinuousQueryListener(cql1);
        r.add([&testCache,&cql1](){ testCache.removeContinuousQueryListener(cql1);});

		sample_bank_account::User_Address a;
		sample_bank_account::User user1;
		user1.set_id(3);
		user1.set_name("Tom");
		user1.set_surname("Cat");
		user1.set_gender(sample_bank_account::User_Gender_MALE);
		sample_bank_account::User_Address * addr = user1.add_addresses();
		addr->set_street("Via Roma");
		addr->set_number(3);
		addr->set_postcode("202020");
		testCache.put(3, user1);

		user1.set_id(4);
		user1.set_name("Jerry");
		user1.set_surname("Mouse");
		user1.set_gender(sample_bank_account::User_Gender_MALE);
		testCache.put(4, user1);

		user1.set_id(4);
		user1.set_name("Mickey");
		testCache.put(4, user1);

		testCache.remove(3);

		if (std::future_status::timeout
				== promise.get_future().wait_for(std::chrono::seconds(30))) {
			std::cout << "FAIL: Continuous query events (Timeout)"
					<< std::endl;
			return -1;
		}

		if (createdCount != 2 || changedCount != 1 || removedCount != 1) {
			std::cout
			<< "FAIL: (createdCount, changedCount, removedCount) is  ("
					<< createdCount << ", " << changedCount << ", "
					<< removedCount << ")" << "  should be (2,1,1)"
					<< std::endl;
			return -1;
		}
	}

	ContinuousQueryListener<int, sample_bank_account::User> cql2(testCache,
			"from sample_bank_account.User");
	ContinuousQueryListener<int, sample_bank_account::User> cql_where(testCache,
			"from sample_bank_account.User where name='Mickey'");
	{
		ResourceManager r;
		std::cout << "Testing multiple queries: from sample_bank_account.User" << std::endl;
		std::cout << "                          from sample_bank_account.User where name='Mickey'" << std::endl;

		testCache.clear();

		std::promise<void> promise;
		int createdCount = 0, changedCount = 0, removedCount = 0;

		std::function<void(int, sample_bank_account::User)> join =
				[&promise, &createdCount](int k, sample_bank_account::User u)
				{
					std::cout << "JOINING: key="<< u.id() << " value="<< u.name() << std::endl;
					++createdCount;
				};

		std::function<void(int, sample_bank_account::User)> leave =
				[&promise, &removedCount](int k, sample_bank_account::User u)
				{
					std::cout << "LEAVING: key="<< u.id() << " value="<< u.name() << std::endl;
					++removedCount;
                    if (k==1) {  // Removing 1 is the last operation
				        promise.set_value();
                    }
				};

		std::function<void(int, sample_bank_account::User)> change =
				[&promise, &changedCount](int k, sample_bank_account::User u)
				{
					std::cout << "CHANGING: key="<< u.id() << " value="<< u.name() << std::endl;
					++changedCount;
				};

		cql2.setJoiningListener(join);
		cql2.setLeavingListener(leave);
		cql2.setUpdatedListener(change);

		testCache.addContinuousQueryListener(cql2);
        r.add([&testCache,&cql2](){ testCache.removeContinuousQueryListener(cql2);});

		int createdCount_where = 0, changedCount_where = 0, removedCount_where = 0;

		std::function<void(int, sample_bank_account::User)> join_where =
				[&promise, &createdCount_where](int k, sample_bank_account::User u)
				{
					std::cout << "JOINING WHERE: key="<< u.id() << " value="<< u.name() << std::endl;
					++createdCount_where;
				};

		std::function<void(int, sample_bank_account::User)> leave_where =
				[&promise, &removedCount_where](int k, sample_bank_account::User u)
				{
					std::cout << "LEAVING WHERE: key="<< u.id() << " value="<< u.name() << std::endl;
					++removedCount_where;
				};

		std::function<void(int, sample_bank_account::User)> change_where =
				[&promise, &changedCount_where](int k, sample_bank_account::User u)
				{
					std::cout << "CHANGING WHERE: key="<< u.id() << " value="<< u.name() << std::endl;
					++changedCount_where;
				};

		cql_where.setJoiningListener(join_where);
		cql_where.setLeavingListener(leave_where);
		cql_where.setUpdatedListener(change_where);
		testCache.addContinuousQueryListener(cql_where);
        r.add([&testCache,&cql_where](){ testCache.removeContinuousQueryListener(cql_where);});

		sample_bank_account::User_Address a;
		sample_bank_account::User user1;
		user1.set_id(1);
		user1.set_name("Tom");
		user1.set_surname("Cat");
		user1.set_gender(sample_bank_account::User_Gender_MALE);
		sample_bank_account::User_Address * addr = user1.add_addresses();
		addr->set_street("Via Roma");
		addr->set_number(3);
		addr->set_postcode("202020");
		testCache.put(1, user1);

		user1.set_id(2);
		user1.set_name("Jerry");
		user1.set_surname("Mouse");
		user1.set_gender(sample_bank_account::User_Gender_MALE);
		testCache.put(2, user1);

		user1.set_name("Mickey");
		testCache.put(2, user1);

		user1.set_age(100);
		testCache.put(2, user1);

		testCache.remove(2);

		testCache.remove(1);  // threads sync. Removing 1 completes the promise

		if (std::future_status::timeout
				== promise.get_future().wait_for(std::chrono::seconds(30))) {
			std::cout << "FAIL: Continuous query events (Timeout)" << std::endl;
			return -1;
		}

		if (createdCount != 2 || changedCount != 2 || removedCount != 2) {
			std::cout
			<< "FAIL: (createdCount,changedCount,removedCount) is  ("
					<< createdCount << ", " << changedCount << ", "
					<< removedCount << ")" << "  should be (2,2,2)"
					<< std::endl;
			return -1;
		}
		if (createdCount_where != 1 || changedCount_where != 1 || removedCount_where != 1) {
			std::cout
			<< "FAIL: (createdCount_where,changedCount_where,removedCount_where) is  ("
					<< createdCount_where << ", " << changedCount_where << ", "
					<< removedCount_where << ")" << "  should be (1,1,1)"
					<< std::endl;
			return -1;
		}
	}

	std::cout << "PASS: continuous query" << std::endl;
	return result;

}

