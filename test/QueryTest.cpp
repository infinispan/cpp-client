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
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable:4267 4244)
#endif
#include "query.pb.h"
#if _MSC_VER
#pragma warning(pop)
#endif
#include "infinispan/hotrod/QueryUtils.h"
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


int main(int argc, char** argv) {

    int result=0;
	std::cout << "Tests for Query" << std::endl;
    ConfigurationBuilder builder;
    builder.addServer().host(argc > 1 ? argv[1] : "127.0.0.1").port(argc > 2 ? atoi(argv[2]) : 11222);
    builder.protocolVersion(Configuration::PROTOCOL_VERSION_24);
//        builder.balancingStrategyProducer(transport::MyRoundRobinBalancingStrategy::newInstance);
    builder.balancingStrategyProducer(nullptr);
    RemoteCacheManager cacheManager(builder.build(), false);
    cacheManager.start();
    //initialize server-side serialization
    auto *km = new BasicTypesProtoStreamMarshaller<std::string>();
    auto *vm = new BasicTypesProtoStreamMarshaller<std::string>();

    RemoteCache<std::string, std::string> metadataCache = cacheManager.getCache<std::string, std::string>(
    		km, &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy,PROTOBUF_METADATA_CACHE_NAME, false);


    metadataCache.put("sample_bank_account/bank.proto", read("query_proto/bank.proto"));
    if (metadataCache.containsKey(ERRORS_KEY_SUFFIX))
	{
      std::cerr << "fail: error in registering .proto model" << std::endl;
      result=-1;
      return result;
	}

    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> testCache = cacheManager.getCache<int, sample_bank_account::User>(
    		testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy, "namedCache", false);
    testCache.clear();
    sample_bank_account::User_Address a;
    sample_bank_account::User user1;
    user1.set_id(3);
    user1.set_name("Tom");
    user1.set_surname("Cat");
    user1.set_gender(sample_bank_account::User_Gender_MALE);
    sample_bank_account::User_Address * addr= user1.add_addresses();
    addr->set_street("Via Roma");
    addr->set_number(3);
    addr->set_postcode("202020");
    testCache.put(3, user1);

    user1.set_id(4);
    user1.set_name("Jerry");
    user1.set_surname("Mouse");
    user1.set_gender(sample_bank_account::User_Gender_MALE);
    testCache.put(4, user1);

	{
		QueryRequest qr;
		qr.set_jpqlstring("from sample_bank_account.User");
		QueryResponse resp = testCache.query(qr);

		std::vector<sample_bank_account::User> res;
		if (!unwrapResults(resp, res)) {
			std::cerr << "fail: found unexpected projection in resultset"
					<< std::endl;
			result = -1;
			return result;
		}

		if (res.size() != 2) {
			std::cerr << "fail: expected 2 got " << res.size() << std::endl;
			result = -1;
			return result;
		}

		for (unsigned int i = 0; i < res.size(); i++) {
			std::cout << "User(id=" << res[i].id() << ",name=" << res[i].name()
					<< ",surname=" << res[i].surname() << ")" << std::endl;
		}
	}
#if !defined (_MSC_VER) || (_MSC_VER>=1800)
	{
		QueryRequest qr;
		qr.set_jpqlstring(
				"select u.name, u.surname from sample_bank_account.User u");
		QueryResponse resp = testCache.query(qr);

		std::vector<std::tuple<std::string, std::string> > prjRes;

		if (!unwrapProjection(resp, prjRes)) {
			std::cerr << "fail: projection not found" << std::endl;
			result = -1;
			return result;
		}

		for (unsigned int i = 0; i < prjRes.size(); i++) {
			std::cout << std::get < 0 > (prjRes[i]) << "  " << std::get < 1
					> (prjRes[i]) << "  " << std::endl;
		}
	}

	{
		QueryRequest qr;
		qr.set_jpqlstring(
				"select u.surname, u.id from sample_bank_account.User u");
		QueryResponse resp = testCache.query(qr);

		std::vector<std::tuple<std::string, int> > prjRes;

		if (!unwrapProjection(resp, prjRes)) {
			std::cerr << "fail: projection not found" << std::endl;
			result = -1;
			return result;
		}

		for (unsigned int i = 0; i < prjRes.size(); i++) {
			std::cout << std::get < 0 > (prjRes[i]) << "  " << std::get < 1
					> (prjRes[i]) << "  " << std::endl;
		}
	}
#endif
	{
		QueryRequest qr;
		qr.set_jpqlstring(
				"select count(u.surname) from sample_bank_account.User u");
		QueryResponse resp = testCache.query(qr);

		int i = unwrapSingleResult<int>(resp);

			std::cout << "result is: " << i << std::endl;
	}


	return result;
}

