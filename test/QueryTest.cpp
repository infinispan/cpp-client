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

    ResourceManager rMain;
    rMain.add([&cacheManager] { cacheManager.stop();});

    metadataCache.put("sample_bank_account/bank.proto", read("query_proto/bank.proto"));
    if (metadataCache.containsKey(ERRORS_KEY_SUFFIX))
    {
      std::cerr << "fail: error in registering .proto model" << std::endl;
      result=-1;
      return result;
    }

    rMain.add([&metadataCache] { metadataCache.remove("sample_bank_account/bank.proto");});

    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> testCache = cacheManager.getCache<int, sample_bank_account::User>(
            testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy, "queryCache", false);
    testCache.clear();
    sample_bank_account::User user1;
    user1.set_id(3);
    user1.set_name("Tom");
    user1.set_surname("Cat");
    user1.set_gender(sample_bank_account::User_Gender_MALE);
    user1.add_accountids(3);
    sample_bank_account::User_Address * addr= user1.add_addresses();
    addr->set_street("Rome St.");
    addr->set_number(3);
    addr->set_postcode("202020");
    testCache.put(3, user1);

    user1.set_id(4);
    user1.set_name("Jerry");
    user1.set_surname("Mouse");
    user1.set_gender(sample_bank_account::User_Gender_MALE);
    user1.clear_accountids();
    user1.add_accountids(2);
    testCache.put(4, user1);
    std::string s;
    // Check if standard get still work with protobuf marshaller
    sample_bank_account::User * userResult1= testCache.get(4);
    if (userResult1->name().compare(user1.name())) {
        std::cerr << "fail: got troubles with .get() method"
                << std::endl;
        result = -1;
        delete(userResult1);
        return result;
    }
    delete(userResult1);

    user1.set_id(5);
    user1.set_name("Atom");
    user1.set_surname("Ant");
    user1.set_gender(sample_bank_account::User_Gender_MALE);
    user1.clear_accountids();
    user1.add_accountids(2);
    user1.add_accountids(1);
    addr= user1.add_addresses();
    addr->set_street("Rome St.");
    addr->set_number(3);
    addr->set_postcode("212121");
    testCache.put(5, user1);

    user1.set_id(6);
    user1.set_name("Secret");
    user1.set_surname("Squirrel");
    user1.set_gender(sample_bank_account::User_Gender_MALE);
    user1.clear_accountids();
    user1.add_accountids(1);
    addr= user1.add_addresses();
    addr->set_street("Turin St.");
    addr->set_number(5);
    addr->set_postcode("212121");
    testCache.put(6, user1);


    {
        QueryRequest qr;
        // set_jpqlstring will be soon deprecated use set_querystring
        // qr.set_jpqlstring("from sample_bank_account.User");
        std::cout << "querystring=from sample_bank_account.User" << std::endl;
        qr.set_querystring("from sample_bank_account.User");
        QueryResponse resp = testCache.query(qr);

        std::vector<sample_bank_account::User> res;
        if (!unwrapResults(resp, res)) {
            std::cerr << "fail: found unexpected projection in resultset"
                    << std::endl;
            result = -1;
            return result;
        }

        if (res.size() != 4) {
            std::cerr << "fail: expected 4 got " << res.size() << std::endl;
            result = -1;
            return result;
        }

        for (unsigned int i = 0; i < res.size(); i++) {
            std::cout << "User(id=" << res[i].id() << ",name=" << res[i].name()
                    << ",surname=" << res[i].surname() << ")" << std::endl;
        }

        std::cout << "querystring=from sample_bank_account.User u where u.accountIds = 2 or u.accountIds = 3 order by u.id desc" << std::endl;
        qr.set_querystring("from sample_bank_account.User u where u.accountIds = 2 or u.accountIds = 3 order by u.id desc");
        resp = testCache.query(qr);
        res.clear();
        if (!unwrapResults(resp, res)) {
            std::cerr << "fail: found unexpected projection in resultset"
                    << std::endl;
            result = -1;
            return result;
        }

        if (res.size() != 3) {
            std::cerr << "fail: expected 3 got " << res.size() << std::endl;
            result = -1;
            return result;
        }

        for (unsigned int i = 0; i < res.size(); i++) {
            std::cout << "User(id=" << res[i].id() << ",name=" << res[i].name()
                    << ",surname=" << res[i].surname() << ")" << std::endl;
            if (i>0 && res[i].id() > res[i-1].id())
            {
                std::cerr << "fail: desc sorting not respected" << std::endl;
                result = -1;
                return result;
            }
        }

        std::cout << "querystring=from sample_bank_account.User u where u.accountIds = 2 or u.accountIds = 3 order by u.id asc" << std::endl;
        qr.set_querystring("from sample_bank_account.User u where u.accountIds = 2 or u.accountIds = 3 order by u.id asc");
        resp = testCache.query(qr);
        res.clear();
        if (!unwrapResults(resp, res)) {
            std::cerr << "fail: found unexpected projection in resultset"
                    << std::endl;
            result = -1;
            return result;
        }

        if (res.size() != 3) {
            std::cerr << "fail: expected 3 got " << res.size() << std::endl;
            result = -1;
            return result;
        }

        for (unsigned int i = 0; i < res.size(); i++) {
            std::cout << "User(id=" << res[i].id() << ",name=" << res[i].name()
                    << ",surname=" << res[i].surname() << ")" << std::endl;
            if (i>0 && res[i].id() < res[i-1].id())
            {
                std::cerr << "fail: asc sorting not respected" << std::endl;
                result = -1;
                return result;
            }
        }

    }
#if !defined (_MSC_VER) || (_MSC_VER>=1800)
    {
        QueryRequest qr;
        // set_jpqlstring will be soon deprecated use set_querystring
        // qr.set_jpqlstring("select u.name, u.surname from sample_bank_account.User u");
        std::cout << "querystring=select u.name, u.surname from sample_bank_account.User u" << std::endl;
        qr.set_querystring("select u.name, u.surname from sample_bank_account.User u");
        QueryResponse resp = testCache.query(qr);

        std::vector<std::tuple<std::string, std::string> > prjRes;

        if (!unwrapProjection(resp, prjRes)) {
            std::cerr << "fail: projection not found" << std::endl;
            result = -1;
            return result;
        }

        for (unsigned int i = 0; i < prjRes.size(); i++) {
            std::cout << "(User.name, User.surname)=(" << std::get < 0 > (prjRes[i]) << "," << std::get < 1
                    > (prjRes[i]) << ")" << std::endl;
        }
    }

    {
        QueryRequest qr;
        // set_jpqlstring will be soon deprecated use set_querystring
        // qr.set_jpqlstring("select u.surname, u.id from sample_bank_account.User u");
        std::cout << "querystring=select u.surname, u.id from sample_bank_account.User u" << std::endl;
        qr.set_querystring("select u.surname, u.id from sample_bank_account.User u");
        QueryResponse resp = testCache.query(qr);

        std::vector<std::tuple<std::string, int> > prjRes;

        if (!unwrapProjection(resp, prjRes)) {
            std::cerr << "fail: projection not found" << std::endl;
            result = -1;
            return result;
        }

        for (unsigned int i = 0; i < prjRes.size(); i++) {
            std::cout << "(User.name, User.id)=(" << std::get < 0 > (prjRes[i]) << "," << std::get < 1
                    > (prjRes[i]) << ")" << std::endl;
        }
    }
#endif
    {
        QueryRequest qr;
        // set_jpqlstring will be soon deprecated use set_querystring
        // qr.set_jpqlstring("select count(u.surname) from sample_bank_account.User u");
        std::cout << "querystring=select count(u.surname) from sample_bank_account.User u" << std::endl;
        qr.set_querystring("select count(u.surname) from sample_bank_account.User u");
        QueryResponse resp = testCache.query(qr);

        int i = unwrapSingleResult<int>(resp);
        std::cout << "count(u.surname)=" << i << std::endl;
        if (i!=4) {
            std::cerr << "fail: expected 4 got "<< i << std::endl;
            result = -1;
            return result;
        }
    }

    {
        QueryRequest qr;
        // set_jpqlstring will be soon deprecated use set_querystring
        // qr.set_jpqlstring("select count(u.surname) from sample_bank_account.User u");
        std::cout << "querystring=select count(u.surname) from sample_bank_account.User u where id=-1" << std::endl;
        qr.set_querystring("select count(u.surname) from sample_bank_account.User u where id=-1");
        QueryResponse resp = testCache.query(qr);

        int i = unwrapSingleResult<int>(resp);

        std::cout << "count(u.surname where id=-1)=" << i << std::endl;
        if (i!=0) {
            std::cerr << "fail: expected 0 got "<< i << std::endl;
            result = -1;
            return result;
        }
    }

    return result;
}

