#include <hotrod/sys/Runnable.h>
#include <hotrod/sys/Thread.h>
#include <hotrod/sys/Mutex.h>
#include <hotrod/sys/Condition.h>
#include <hotrod/sys/RunOnce.h>
#include <hotrod/sys/Log.h>
#include "hotrod/impl/transport/tcp/InetSocketAddress.h"

#include <iostream>
#include <iterator>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

volatile int passFail = 0;
volatile int available = 0;
volatile bool waitersDone = false;

using namespace infinispan::hotrod::sys;
using namespace infinispan::hotrod::transport;

class Sleeper : public Runnable {
  public:
    Sleeper(std::string name, unsigned len, int &s) : me(name), sleepTime(len), status(s) {}
    void run() {
        Thread::sleep(sleepTime);
        status = 1;
        INFO("%s done", me.c_str());
    }
  private:
    std::string me;
    unsigned sleepTime;
    int& status;
};


void threadTest() {
    int st1 = 0;
    int st2 = 0;
    INFO("starting threads");
    Sleeper s1("sleeper 1", 500, st1);
    Sleeper s2("sleeper 2", 1500, st2);
    Thread t1((Runnable &) s1);
    Thread t2((Runnable &) s2);
    if ((st1 != 0) || (st2 != 0)) {
        passFail = 1;
        ERROR("initial sleep fail");
    }
    t1.join();
    t2.join();
    if ((st1 != 1) || (st2 != 1)) {
        passFail = 1;
        ERROR("post yield status fail (%d) (%d)", st1, st2);
    }
    return;
}

class Waiter : public Runnable {
  public:
    Waiter(std::string name, Mutex& m, Condition &c, bool b) : me(name), lock(m), condition(c), timed(b)  {}
    void run() {
        if (timed) {
            ScopedLock<Mutex> l(lock);
            passFail = 5;
            // test is expected to timeout
            if (condition.wait(lock, 2000000)) {
                ERROR("%s timed waiter fail", me.c_str());
                return;
            }
            passFail = 0;
            INFO("%s timed waiter pass", me.c_str());
            return;
        }

        ScopedLock<Mutex> l(lock);
        while(1) {
            if (waitersDone)
                return;
            if (available > 0) {
                available--;
                INFO("%s consumed one", me.c_str());
                ScopedUnlock<Mutex> ul(lock);
                Thread::sleep(100);
            }
            else {
                condition.wait(lock);
            }
        }
    }
  private:
    std::string me;
    Mutex& lock;
    Condition& condition;
    bool timed;
};


void syncTest() {
    Mutex lock;
    Condition condition;
    INFO("starting sync threads");
    Waiter tw("timed waiter", lock, condition, true);
    Waiter w1("waiter1", lock, condition, false);
    Waiter w2("waiter2", lock, condition, false);

    Thread t1((Runnable &) w1);
    Thread t2((Runnable &) w2);
    Thread t3((Runnable &) tw);

    // allow tw to time out
    t3.join();
    if (passFail)
        return;

    {
        ScopedLock<Mutex> l(lock);
        available = 1;
        condition.notify();
    }

    Thread::sleep(500);
    {
        ScopedLock<Mutex> l(lock);
        if (available != 0) {
            passFail = 1;
            ERROR("single notify fail");
            return;
        }
        INFO("single notify pass");
        available = 5;
        condition.notifyAll();
    }

    Thread::sleep(1000);
    {
        ScopedLock<Mutex> l(lock);
        if (available != 0) {
            passFail = 1;
            ERROR("notifyAll fail");
            return;
        }
        waitersDone = true;
        condition.notifyAll();
    }

    t1.join();
    t2.join();
    INFO("Sync test passed");
    return;
}


int testInitStatus = 0;
int testInitRunning = 0;
Mutex testInitMutex;

void testInitFunc() {
    {
        ScopedLock<Mutex> l(testInitMutex);
        if (testInitStatus || testInitRunning)
            passFail = 1;
        testInitRunning = 1;
    }
    Thread::sleep(1000);
    {
        ScopedLock<Mutex> l(testInitMutex);
        testInitStatus++;
        testInitRunning = 0;
    }

}

RunOnce testInitializer(&testInitFunc);

class UseTestInit : public Runnable {
  public:
    UseTestInit(std::string name) : me(name) {}
    void run() {
        INFO("%s starting", me.c_str());
        testInitializer.runOnce();
        // no thread should see 0 or >1
        if (testInitStatus != 1) passFail = 1;
        INFO("%s done", me.c_str());
    }
  private:
    std::string me;
};

void runOnceTest() {
    INFO("starting once test");
    UseTestInit r1("run once worker 1");
    UseTestInit r2("run once worker 2");
    Thread t1((Runnable &) r1);
    Thread t2((Runnable &) r2);
    t1.join();
    t2.join();
    if (testInitStatus != 1) passFail = 1;
    if (passFail == 1) {
        ERROR("RunOnce test failed %d", testInitStatus);
    } else {
        INFO("RunOnce test passed");
    }
    return;
}

void testTopologyChangeResponse() {

    std::ostringstream oss;
    std::map<InetSocketAddress, std::set<int32_t> > map;
    std::set<int32_t> *hashesPtr;
    uint32_t clusterSize = 10;
    for (uint32_t i = 0; i < clusterSize; i++) {
        oss << i;
        std::string host = "host" + oss.str();
        int16_t port = i;
        int32_t hashCode = i;
        InetSocketAddress address(host, port);
        if (address.getHostname().compare("host01") == 0) {
            InetSocketAddress duplicate("host012", 2);
            address = duplicate;
        }

        std::map<InetSocketAddress, std::set<int32_t> >::iterator it =
                map.find(address);
        if (it == map.end()) {
            std::set<int32_t> hashes;
            hashes.insert(hashCode);
            map.insert(
                    std::pair<InetSocketAddress, std::set<int32_t> >(address,
                            hashes));
        } else {
            hashCode = 12345; //some totally different hash for duplicate
            it->second.insert(hashCode);
        }
    }

    //iterate over map and collects hosts in a vector
    std::vector<InetSocketAddress> socketAddresses;
    for (std::map<InetSocketAddress, std::set<int32_t> >::iterator it =
            map.begin(); it != map.end(); ++it) {
        socketAddresses.push_back(it->first);
    }

    if(socketAddresses.size() != 9) passFail =1;;

    //iterate over vector of InetSocketAddress and print them out
    for (std::vector<InetSocketAddress>::const_iterator it =
            socketAddresses.begin(); it != socketAddresses.end(); ++it) {
        INFO("%s", it->getHostname().c_str());
    }

    //create duplicate reference from above
    InetSocketAddress desired_host("host012", 2);
    std::map<InetSocketAddress, std::set<int32_t> >::iterator iter0 =
            map.find(desired_host);

    //set should have two elements
    if (iter0 != map.end()) {
        hashesPtr = &iter0->second;
        if(hashesPtr->size() != 2) passFail =1;;
        std::set<int32_t>::iterator iter;
        for (iter = hashesPtr->begin(); iter != hashesPtr->end(); ++iter) {
            INFO("For %s:%d found hash %d", desired_host.getHostname().c_str(), desired_host.getPort(), *iter);
        }
    }

    //set should have one element
    InetSocketAddress desired_host2("host0", 0);
    std::map<InetSocketAddress, std::set<int32_t> >::iterator iter2 =
            map.find(desired_host2);
    if (iter2 != map.end()) {
        hashesPtr = &iter2->second;
        if(hashesPtr->size() != 1) passFail =1;
        std::set<int32_t>::iterator iter3;
        for (iter3 = hashesPtr->begin(); iter3 != hashesPtr->end(); ++iter3) {
            INFO("For %s:%d found hash %d", desired_host2.getHostname().c_str(), desired_host2.getPort(), *iter3);
        }
    }
    if(map.size() != 9) passFail =1;
    INFO("testTopologyChangeResponse tests passed");
}

std::pair<std::vector<InetSocketAddress>,std::vector<InetSocketAddress> > updateServers(std::vector<InetSocketAddress>& newServers, std::vector<InetSocketAddress>& servers) {
    std::vector<InetSocketAddress> addedServers;
    std::sort (newServers.begin(),newServers.end());
    std::sort (servers.begin(), servers.end());

    std::set_difference(newServers.begin(), newServers.end(),
                servers.begin(), servers.end(),
            std::inserter(addedServers, addedServers.end()));

    std::vector<InetSocketAddress> failedServers;
    std::sort (newServers.begin(),newServers.end());
    std::sort (servers.begin(), servers.end());

    std::set_difference(servers.begin(), servers.end(),
                newServers.begin(), newServers.end(),
            std::inserter(failedServers, failedServers.end()));

    if (failedServers.empty() && newServers.empty()) {
        return std::pair<std::vector<InetSocketAddress>,std::vector<InetSocketAddress> >(addedServers, failedServers);;
    }

    //1. first add new servers. For servers that went down, the returned transport will fail for now
    for(std::vector<InetSocketAddress>::const_iterator it = addedServers.begin();it != addedServers.end();++it){
       INFO("Adding to pool %s", it->getHostname().c_str());
    }

    //2. now set the server list to the active list of servers. All the active servers (potentially together with some
    // failed servers) are in the pool now. But after this, the pool won't be asked for connections to failed servers,
    // as the balancer will only know about the active servers
    //balancer->setServers(newServers);

    //3. Now just remove failed servers
    for (std::vector<InetSocketAddress>::const_iterator it = failedServers.begin(); it != failedServers.end(); ++it) {
        INFO("Clearing pool with %s", it->getHostname().c_str());
    }

    //servers.clear();
    return std::pair<std::vector<InetSocketAddress>,std::vector<InetSocketAddress> >(addedServers, failedServers);
}


int main(int, char**) {
    threadTest();
    syncTest();
    runOnceTest();
    testTopologyChangeResponse();

    std::vector<InetSocketAddress> oldAddresses;
    std::vector<InetSocketAddress> newAddresses;

    InetSocketAddress address4("a", 1);
    InetSocketAddress address6("g", 0);
    InetSocketAddress address7("f", 6);
    oldAddresses.push_back(address4);
    oldAddresses.push_back(address6);
    oldAddresses.push_back(address7);

    InetSocketAddress address0("g", 0);
    InetSocketAddress address1("a", 1);
    InetSocketAddress address2("b", 2);
    InetSocketAddress address3("c", 3);
    newAddresses.push_back(address2);
    newAddresses.push_back(address0);
    newAddresses.push_back(address1);
    newAddresses.push_back(address3);

    std::pair<std::vector<InetSocketAddress>, std::vector<InetSocketAddress> >
            res = updateServers(newAddresses, oldAddresses);

    if (newAddresses.size() != 4 || oldAddresses.size() != 3) {
        passFail = 1;
    }

    if (res.first.size() != 2 || res.second.size() != 1) {
        passFail = 1;
    }
    if (passFail == 0) {
        INFO("updateServers passed");
    }

    return passFail;
}
