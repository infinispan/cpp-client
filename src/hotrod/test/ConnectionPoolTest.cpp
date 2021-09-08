#include "hotrod/impl/transport/tcp/ConnectionPool.h"
#include "hotrod/impl/transport/tcp/TransportObjectFactory.h"
#include "hotrod/sys/Log.h"
#include "hotrod/sys/Runnable.h"
#include "hotrod/sys/Thread.h"
#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/ConfigurationBuilder.h"
#include <assert.h>

using namespace infinispan::hotrod::operations;
using namespace infinispan::hotrod::sys;
using namespace infinispan::hotrod::transport;
using namespace infinispan::hotrod;

#define UNUSED(x) (void)(x)
#define UNLIMITED -1

class TestTransport: public TcpTransport {

public:
    TestTransport() :
            TcpTransport() {
    }

    void flush() {
    }

    void writeByte(uint8_t var) {
        UNUSED(var);
    }

    void writeVInt(uint32_t var) {
        UNUSED(var);
    }

    void writeVLong(uint64_t var) {
        UNUSED(var);
    }

    uint8_t readByte() {
        return 0;
    }

    uint32_t readVInt() {
        return 0;
    }

    uint64_t readVLong() {
        return 0;
    }

    void release() {

    }

    void invalidate() {

    }

protected:

    std::vector<char> readBytes(uint32_t size) {
        UNUSED(size);
        return std::vector<char>();
    }
};

class TestTransportFactory: public AbstractObjectFactory {
public:
    TestTransportFactory() :
            AbstractObjectFactory() {
    }

    TcpTransport& makeObject(const InetSocketAddress&) {
        return *(new TestTransport);
    }

    void destroyObject(const InetSocketAddress&, TcpTransport& transport) {
        delete &transport;
    }

    PingResult ping(TcpTransport&) {
        return SUCCESS;
    }

};

class BorrowTransportRunnable: public Runnable {

public:

    BorrowTransportRunnable(ConnectionPool* pool_, InetSocketAddress* address_) :
            pickedTransport(false), pool(pool_), address(address_), transport(
            NULL) {
    }

    void run() {
        transport = &pool->borrowObject(*address);
        pickedTransport = true;
    }

    bool hasPickedTransport() {
        return pickedTransport;
    }

    TcpTransport* getTransport() {
        return transport;
    }
    ;

private:
    bool pickedTransport;
    ConnectionPool* pool;
    InetSocketAddress* address;
    TcpTransport* transport;
};

Configuration createConfiguration(int minIdle, int maxActive, int maxTotal) {
    ConfigurationBuilder builder;
    builder.connectionPool().minIdle(minIdle).maxActive(maxActive).maxTotal(maxTotal);
    return builder.build();
}

void deleteArray(InetSocketAddress** array, int size) {
    for (int i = 0; i < size; ++i) {
        delete array[i];
        array[i] = NULL;
    }
    delete[] array; //impl details: it was allocated with new ...[];
}

HR_EXPORT void testMinIdle() {
    const int minIdle = 3;
    const int numberOfKeys = 3;
    const int borrowSize = 4;

    std::shared_ptr<TestTransportFactory> factory = std::shared_ptr<TestTransportFactory>(new TestTransportFactory);
    Configuration config = createConfiguration(minIdle, UNLIMITED, UNLIMITED);
    ConnectionPool* pool = new ConnectionPool(factory, config.getConnectionPoolConfiguration());

    InetSocketAddress** addrs = new InetSocketAddress*[numberOfKeys];
    for (int i = 0; i < numberOfKeys; ++i) {
        InetSocketAddress* addr = new InetSocketAddress("127.0.0.1", 1024 + i);
        addrs[i] = addr;
        pool->preparePool(*addr);
        assert(pool->getNumActive(*addr) == 0);
        assert(pool->getNumIdle(*addr) == minIdle);
    }

    assert(pool->getNumIdle() == minIdle * numberOfKeys);
    assert(pool->getNumActive() == 0);

    TcpTransport** trps = new TcpTransport*[borrowSize];

    for (int i = 0; i < borrowSize; ++i) {
        TcpTransport& t = pool->borrowObject(*(addrs[0]));
        trps[i] = &t;
    }

    assert(pool->getNumActive(*(addrs[0])) == borrowSize);
    assert(pool->getNumIdle(*(addrs[0])) == 0);
    assert(pool->getNumActive(*(addrs[1])) == 0);
    assert(pool->getNumIdle(*(addrs[1])) == minIdle);
    assert(pool->getNumActive(*(addrs[2])) == 0);
    assert(pool->getNumIdle(*(addrs[2])) == minIdle);

    assert(pool->getNumIdle() == 2 * minIdle);
    assert(pool->getNumActive() == borrowSize);

    for (int i = 0; i < borrowSize; ++i) {
        TcpTransport& t = *(trps[i]);
        pool->returnObject(*(addrs[0]), t);
        trps[i] = NULL;
    }

    assert(pool->getNumActive(*(addrs[0])) == 0);
    assert(pool->getNumIdle(*(addrs[0])) == borrowSize);
    assert(pool->getNumActive(*(addrs[1])) == 0);
    assert(pool->getNumIdle(*(addrs[1])) == minIdle);
    assert(pool->getNumActive(*(addrs[2])) == 0);
    assert(pool->getNumIdle(*(addrs[2])) == minIdle);

    assert(pool->getNumIdle() == (2 * minIdle + borrowSize));
    assert(pool->getNumActive() == 0);

    deleteArray(addrs, numberOfKeys);
    delete pool;
    delete[] trps;
}

HR_EXPORT void testMaxActive() {
    const int minIdle = 1;
    const int maxActive = 3;
    const int numberOfKeys = 3;

    std::shared_ptr<TestTransportFactory> factory = std::shared_ptr<TestTransportFactory>(new TestTransportFactory);
    Configuration config = createConfiguration(minIdle, maxActive, UNLIMITED);
    ConnectionPool* pool = new ConnectionPool(factory, config.getConnectionPoolConfiguration());

    InetSocketAddress** addrs = new InetSocketAddress*[numberOfKeys];
    for (int i = 0; i < numberOfKeys; ++i) {
        InetSocketAddress* addr = new InetSocketAddress("127.0.0.1", 1024 + i);
        addrs[i] = addr;
        pool->preparePool(*addr);
        assert(pool->getNumActive(*addr) == 0);
        assert(pool->getNumIdle(*addr) == minIdle);
    }

    assert(pool->getNumIdle() == minIdle * numberOfKeys);
    assert(pool->getNumActive() == 0);

    TcpTransport** trps = new TcpTransport*[maxActive];

    for (int i = 0; i < maxActive; ++i) {
        TcpTransport& t = pool->borrowObject(*(addrs[0]));
        trps[i] = &t;
    }

    assert(pool->getNumActive(*(addrs[0])) == maxActive);
    assert(pool->getNumIdle(*(addrs[0])) == 0);

    BorrowTransportRunnable* runnable = new BorrowTransportRunnable(pool, addrs[0]);

    Thread* thread = new Thread(runnable);
    Thread::sleep(5000);
    assert(!runnable->hasPickedTransport());

    assert(pool->getNumActive(*(addrs[0])) == maxActive);
    assert(pool->getNumIdle(*(addrs[0])) == 0);

    pool->returnObject(*addrs[0], *trps[0]);

    thread->join();

    assert(runnable->hasPickedTransport());
    assert(trps[0] == runnable->getTransport()); //it should point to the same object

    assert(pool->getNumActive(*(addrs[0])) == maxActive);
    assert(pool->getNumIdle(*(addrs[0])) == 0);

    for (int i = 0; i < maxActive; ++i) {
        TcpTransport& t = *(trps[i]);
        pool->returnObject(*(addrs[0]), t);
        trps[i] = NULL;
    }

    deleteArray(addrs, numberOfKeys);
    delete[] trps;
    delete pool;
    delete runnable;
    delete thread;
}

HR_EXPORT void testMaxTotal() {
    const int minIdle = 1;
    const int maxActive = 3;
    const int maxTotal = 1;
    const int numberOfKeys = 3;

    std::shared_ptr<TestTransportFactory> factory = std::shared_ptr<TestTransportFactory>(new TestTransportFactory);
    Configuration config = createConfiguration(minIdle, maxActive, maxTotal);
    ConnectionPool* pool = new ConnectionPool(factory, config.getConnectionPoolConfiguration());

    InetSocketAddress** addrs = new InetSocketAddress*[numberOfKeys];
    InetSocketAddress* addr = new InetSocketAddress("127.0.0.1", 1024);
    pool->preparePool(*addr);
    addrs[0] = addr;

    assert(pool->getNumActive(*addr) == 0);
    assert(pool->getNumIdle(*addr) == minIdle);
    assert(pool->getNumIdle() == 1);
    assert(pool->getNumActive() == 0);

    //the second address will have no space to create the idle connection since maxTotal == 1
    addr = new InetSocketAddress("127.0.0.1", 1025);
    pool->preparePool(*addr);
    addrs[1] = addr;

    assert(pool->getNumActive(*addr) == 0);
    assert(pool->getNumIdle(*addr) == 0);
    assert(pool->getNumIdle() == 1);
    assert(pool->getNumActive() == 0);

    //the same thing for 3rd address
    addr = new InetSocketAddress("127.0.0.1", 1026);
    pool->preparePool(*addr);
    addrs[2] = addr;

    assert(pool->getNumActive(*addr) == 0);
    assert(pool->getNumIdle(*addr) == 0);
    assert(pool->getNumIdle() == 1);
    assert(pool->getNumActive() == 0);

    assert(pool->getNumIdle() == 1);
    assert(pool->getNumActive() == 0);

    //pick a transport from 1st address
    addr = addrs[0];
    TcpTransport& tp = pool->borrowObject(*addr);

    //check nothing is violated
    assert(pool->getNumActive(*addr) == 1);
    assert(pool->getNumIdle(*addr) == 0);
    assert(pool->getNumIdle() == 0);
    assert(pool->getNumActive() == 1);

    BorrowTransportRunnable *r1 = new BorrowTransportRunnable(pool, addrs[1]);
    BorrowTransportRunnable *r2 = new BorrowTransportRunnable(pool, addrs[2]);

    Thread *t1 = new Thread(r1);
    Thread *t2 = new Thread(r2);

    Thread::sleep(5000);

    //no progress should be made! max total is reached!
    assert(pool->getNumActive(*addr) == 1);
    assert(pool->getNumIdle(*addr) == 0);

    assert(pool->getNumActive(*(addrs[1])) == 0);
    assert(pool->getNumIdle(*(addrs[1])) == 0);

    assert(pool->getNumActive(*(addrs[2])) == 0);
    assert(pool->getNumIdle(*(addrs[2])) == 0);

    assert(pool->getNumIdle() == 0);
    assert(pool->getNumActive() == 1);

    assert(!r1->hasPickedTransport());
    assert(!r2->hasPickedTransport());

    //should unlock one of them
    pool->returnObject(*addr, tp);
    Thread::sleep(5000);

    bool r1First = false;

    if (r1->hasPickedTransport() && !r2->hasPickedTransport()) {
        r1First = true; //first to pick the transport
        //check max total
        assert(pool->getNumActive(*(addrs[0])) == 0);
        assert(pool->getNumIdle(*(addrs[0])) == 0);

        assert(pool->getNumActive(*(addrs[1])) == 1);
        assert(pool->getNumIdle(*(addrs[1])) == 0);

        assert(pool->getNumActive(*(addrs[2])) == 0);
        assert(pool->getNumIdle(*(addrs[2])) == 0);

        assert(pool->getNumIdle() == 0);
        assert(pool->getNumActive() == 1);

        //check valid transport by randomly invoke a method
        r1->getTransport()->flush();

        pool->returnObject(*(addrs[1]), *r1->getTransport());
    } else if (!r1->hasPickedTransport() && r2->hasPickedTransport()) {
        //check max total
        assert(pool->getNumActive(*(addrs[0])) == 0);
        assert(pool->getNumIdle(*(addrs[0])) == 0);

        assert(pool->getNumActive(*(addrs[1])) == 0);
        assert(pool->getNumIdle(*(addrs[1])) == 0);

        assert(pool->getNumActive(*(addrs[2])) == 1);
        assert(pool->getNumIdle(*(addrs[2])) == 0);

        assert(pool->getNumIdle() == 0);
        assert(pool->getNumActive() == 1);

        //check valid transport by randomly invoke a method
        r2->getTransport()->flush();
        pool->returnObject(*(addrs[2]), *r2->getTransport());
    } else {
        //these assertions should fail!
        assert(!(!r1->hasPickedTransport() && !r2->hasPickedTransport())); //none of then advanced??
        assert(!(r1->hasPickedTransport() && r2->hasPickedTransport())); //both of them adanced??
    }

    Thread::sleep(5000);

    if (r1First) {
        //check max total
        assert(pool->getNumActive(*(addrs[0])) == 0);
        assert(pool->getNumIdle(*(addrs[0])) == 0);

        assert(pool->getNumActive(*(addrs[1])) == 0);
        assert(pool->getNumIdle(*(addrs[1])) == 0);

        assert(pool->getNumActive(*(addrs[2])) == 1);
        assert(pool->getNumIdle(*(addrs[2])) == 0);

        assert(pool->getNumIdle() == 0);
        assert(pool->getNumActive() == 1);

        //check valid transport by randomly invoke a method
        r2->getTransport()->flush();
        pool->returnObject(*(addrs[2]), *r2->getTransport());
    } else {
        //check max total
        assert(pool->getNumActive(*(addrs[0])) == 0);
        assert(pool->getNumIdle(*(addrs[0])) == 0);

        assert(pool->getNumActive(*(addrs[1])) == 1);
        assert(pool->getNumIdle(*(addrs[1])) == 0);

        assert(pool->getNumActive(*(addrs[2])) == 0);
        assert(pool->getNumIdle(*(addrs[2])) == 0);

        assert(pool->getNumIdle() == 0);
        assert(pool->getNumActive() == 1);

        //check valid transport by randomly invoke a method
        r1->getTransport()->flush();

        pool->returnObject(*(addrs[1]), *r1->getTransport());
    }

    t1->join();
    t2->join();

    //check max total
    assert(pool->getNumActive(*(addrs[0])) == 0);
    assert(pool->getNumIdle(*(addrs[0])) == 0);

    assert(pool->getNumActive(*(addrs[1])) == 0);
    assert(pool->getNumIdle(*(addrs[1])) == (r1First ? 0 : 1));

    assert(pool->getNumActive(*(addrs[2])) == 0);
    assert(pool->getNumIdle(*(addrs[2])) == (r1First ? 1 : 0));

    assert(pool->getNumIdle() == 1);
    assert(pool->getNumActive() == 0);

    //cleanup. run with valgrind to make sure that the transport is free'd
    deleteArray(addrs, numberOfKeys);
    delete pool;
    delete r1;
    delete r2;
    delete t1;
    delete t2;
}

HR_EXPORT void testMaxTotal2() {
    const int minIdle = 2;
    const int maxActive = UNLIMITED;
    const int maxTotal = 6;
    const int numberOfKeys = 3;

    std::shared_ptr<TestTransportFactory> factory = std::shared_ptr<TestTransportFactory>(new TestTransportFactory);
    Configuration config = createConfiguration(minIdle, maxActive, maxTotal);
    ConnectionPool* pool = new ConnectionPool(factory, config.getConnectionPoolConfiguration());

    InetSocketAddress** addrs = new InetSocketAddress*[numberOfKeys];
    for (int i = 0; i < numberOfKeys; ++i) {
        InetSocketAddress* addr = new InetSocketAddress("127.0.0.1", 1024 + i);
        addrs[i] = addr;
        pool->preparePool(*addr);
        assert(pool->getNumActive(*addr) == 0);
        assert(pool->getNumIdle(*addr) == minIdle);
    }

    assert(pool->getNumIdle() == minIdle * numberOfKeys);
    assert(pool->getNumActive() == 0);

    TcpTransport** trps = new TcpTransport*[maxTotal];
    for (int i = 0; i < minIdle; ++i) {
        trps[i] = &pool->borrowObject(*addrs[0]);
    }

    //check nothing is violated
    assert(pool->getNumActive(*addrs[0]) == minIdle);
    assert(pool->getNumIdle(*addrs[0]) == 0);
    assert(pool->getNumIdle() == minIdle * (numberOfKeys - 1));
    assert(pool->getNumActive() == minIdle);

    //it should steal from other queues
    trps[2] = &pool->borrowObject(*addrs[0]);
    assert(
            (pool->getNumIdle(*addrs[1]) == (minIdle - 1) && pool->getNumIdle(*addrs[2]) == minIdle)
                    || (pool->getNumIdle(*addrs[1]) == minIdle && pool->getNumIdle(*addrs[2]) == (minIdle - 1)));
    assert(pool->getNumActive(*addrs[0]) == minIdle + 1);

    trps[3] = &pool->borrowObject(*addrs[0]);
    assert(pool->getNumIdle(*addrs[1]) == (minIdle - 1) && pool->getNumIdle(*addrs[2]) == (minIdle - 1));
    assert(pool->getNumActive(*addrs[0]) == minIdle + 2);

    trps[4] = &pool->borrowObject(*addrs[0]);
    assert(
            (pool->getNumIdle(*addrs[1]) == (minIdle - 2) && pool->getNumIdle(*addrs[2]) == (minIdle - 1))
                    || (pool->getNumIdle(*addrs[1]) == (minIdle - 1) && pool->getNumIdle(*addrs[2]) == (minIdle - 2)));
    assert(pool->getNumActive(*addrs[0]) == minIdle + 3);

    trps[5] = &pool->borrowObject(*addrs[0]);
    assert((pool->getNumIdle(*addrs[1]) == (minIdle - 2) && pool->getNumIdle(*addrs[2]) == (minIdle - 2)));
    assert(pool->getNumActive(*addrs[0]) == minIdle + 4);

    for (int i = 0; i < maxTotal; ++i) {
        pool->returnObject(*addrs[0], *trps[i]);
    }

    assert(pool->getNumActive(*addrs[0]) == 0);
    assert(pool->getNumIdle(*addrs[0]) == maxTotal);

    //cleanup. run with valgrind to make sure that the transport is free'd
    deleteArray(addrs, numberOfKeys);
    delete[] trps;
    delete pool;

}

HR_EXPORT void testMaxTotal3() {
    const int minIdle = 6;
    const int maxActive = UNLIMITED;
    const int maxTotal = minIdle;
    const int numberOfKeys = 3;

    std::shared_ptr<TestTransportFactory> factory = std::shared_ptr<TestTransportFactory>(new TestTransportFactory);
    Configuration config = createConfiguration(minIdle, maxActive, maxTotal);
    ConnectionPool* pool = new ConnectionPool(factory, config.getConnectionPoolConfiguration());

    InetSocketAddress** addrs = new InetSocketAddress*[numberOfKeys];
    for (int i = 0; i < numberOfKeys; ++i) {
        addrs[i] = new InetSocketAddress("127.0.0.1", 1024 + i);
    }

    pool->preparePool(*addrs[0]);
    assert(pool->getNumIdle(*addrs[0]) == minIdle);
    assert(pool->getNumActive(*addrs[0]) == 0);

    pool->preparePool(*addrs[1]);
    assert(pool->getNumIdle(*addrs[1]) == 0);
    assert(pool->getNumActive(*addrs[1]) == 0);

    pool->preparePool(*addrs[2]);
    assert(pool->getNumIdle(*addrs[2]) == 0);
    assert(pool->getNumActive(*addrs[2]) == 0);

    assert(pool->getNumIdle() == maxTotal);
    assert(pool->getNumActive() == 0);
    assert((pool->getNumIdle() + pool->getNumActive()) == maxTotal);

    TcpTransport& t = pool->borrowObject(*addrs[1]);

    assert(pool->getNumIdle(*addrs[0]) == (minIdle - 1));
    assert(pool->getNumActive(*addrs[0]) == 0);
    assert(pool->getNumIdle(*addrs[1]) == 0);
    assert(pool->getNumActive(*addrs[1]) == 1);
    assert(pool->getNumIdle(*addrs[2]) == 0);
    assert(pool->getNumActive(*addrs[2]) == 0);
    assert((pool->getNumIdle() + pool->getNumActive()) == maxTotal);

    pool->returnObject(*addrs[1], t);

    assert(pool->getNumIdle(*addrs[0]) == (minIdle - 1));
    assert(pool->getNumActive(*addrs[0]) == 0);
    assert(pool->getNumIdle(*addrs[1]) == 1);
    assert(pool->getNumActive(*addrs[1]) == 0);
    assert(pool->getNumIdle(*addrs[2]) == 0);
    assert(pool->getNumActive(*addrs[2]) == 0);
    assert((pool->getNumIdle() + pool->getNumActive()) == maxTotal);

    //cleanup. run with valgrind to make sure that the transport is free'd
    deleteArray(addrs, numberOfKeys);
    delete pool;
}

HR_EXPORT void testMaxTotal4() {
    const int minIdle = 1;
    const int maxActive = UNLIMITED;
    const int maxTotal = 1;
    const int numberOfKeys = 2;

    std::shared_ptr<TestTransportFactory> factory = std::shared_ptr<TestTransportFactory>(new TestTransportFactory);
    Configuration config = createConfiguration(minIdle, maxActive, maxTotal);
    ConnectionPool* pool = new ConnectionPool(factory, config.getConnectionPoolConfiguration());

    InetSocketAddress** addrs = new InetSocketAddress*[numberOfKeys];
    for (int i = 0; i < numberOfKeys; ++i) {
        addrs[i] = new InetSocketAddress("127.0.0.1", 1024 + i);
    }

    pool->preparePool(*addrs[0]);
    assert(pool->getNumIdle(*addrs[0]) == minIdle);
    assert(pool->getNumActive(*addrs[0]) == 0);

    pool->preparePool(*addrs[1]);
    assert(pool->getNumIdle(*addrs[1]) == 0);
    assert(pool->getNumActive(*addrs[1]) == 0);

    assert(pool->getNumIdle() == maxTotal);
    assert(pool->getNumActive() == 0);
    assert((pool->getNumIdle() + pool->getNumActive()) == maxTotal);

    TcpTransport& t = pool->borrowObject(*addrs[0]);

    assert(pool->getNumIdle(*addrs[0]) == 0);
    assert(pool->getNumActive(*addrs[0]) == 1);
    assert(pool->getNumIdle(*addrs[1]) == 0);
    assert(pool->getNumActive(*addrs[1]) == 0);
    assert((pool->getNumIdle() + pool->getNumActive()) == maxTotal);

    //it should block!
    BorrowTransportRunnable *r1 = new BorrowTransportRunnable(pool, addrs[1]);
    Thread *t1 = new Thread(r1);
    Thread::sleep(5000);

    assert(!r1->hasPickedTransport());
    assert(pool->getNumIdle(*addrs[0]) == 0);
    assert(pool->getNumActive(*addrs[0]) == 1);
    assert(pool->getNumIdle(*addrs[1]) == 0);
    assert(pool->getNumActive(*addrs[1]) == 0);
    assert((pool->getNumIdle() + pool->getNumActive()) == maxTotal);

    pool->invalidateObject(*addrs[0], &t); //it should unlock the thread!
    Thread::sleep(5000);

    assert(r1->hasPickedTransport());
    assert(pool->getNumIdle(*addrs[0]) == 0);
    assert(pool->getNumActive(*addrs[0]) == 0);
    assert(pool->getNumIdle(*addrs[1]) == 0);
    assert(pool->getNumActive(*addrs[1]) == 1);
    assert((pool->getNumIdle() + pool->getNumActive()) == maxTotal);

    pool->invalidateObject(*addrs[1], r1->getTransport());

    t1->join();

    assert(pool->getNumIdle(*addrs[0]) == 0);
    assert(pool->getNumActive(*addrs[0]) == 0);
    assert(pool->getNumIdle(*addrs[1]) == 0);
    assert(pool->getNumActive(*addrs[1]) == 0);
    assert((pool->getNumIdle() + pool->getNumActive()) == 0);

    TcpTransport& t2 = pool->borrowObject(*addrs[0]);

    assert(pool->getNumIdle(*addrs[0]) == 0);
    assert(pool->getNumActive(*addrs[0]) == 1);
    assert(pool->getNumIdle(*addrs[1]) == 0);
    assert(pool->getNumActive(*addrs[1]) == 0);
    assert((pool->getNumIdle() + pool->getNumActive()) == maxTotal);

    pool->returnObject(*addrs[0], t2);

    assert(pool->getNumIdle(*addrs[0]) == 1);
    assert(pool->getNumActive(*addrs[0]) == 0);
    assert(pool->getNumIdle(*addrs[1]) == 0);
    assert(pool->getNumActive(*addrs[1]) == 0);
    assert((pool->getNumIdle() + pool->getNumActive()) == maxTotal);

    //cleanup. run with valgrind to make sure that the transport is free'd
    deleteArray(addrs, numberOfKeys);
    delete pool;
    delete r1;
    delete t1;
}
