#include <infinispan/hotrod/Configuration.h>
#include "infinispan/hotrod/ConfigurationBuilder.h"
#include <infinispan/hotrod/ConnectionPoolConfigurationBuilder.h>
#include <hotrod/impl/transport/tcp/ConnectionPool.h>
#include <hotrod/impl/transport/tcp/TransportObjectFactory.h>

#include <iostream>
#include <thread>
#include <future>
#include <chrono>

using namespace infinispan::hotrod::transport;
using namespace infinispan::hotrod::operations;


// Basic MOC to run the unit tests
class TestTransport: public TcpTransport {
public:
	TestTransport() :
			TcpTransport() {
	}
};

TestTransport arrayOfT[20];
InetSocketAddress addr;
int flag;

class PoolTestObjectFactory: public AbstractObjectFactory {
public:
	int count = 0;
	virtual TcpTransport& makeObject(const InetSocketAddress& address) {
		return arrayOfT[count++];
	}
	virtual void destroyObject(const InetSocketAddress& address,
			TcpTransport& transport) {
	}
	virtual operations::PingResult ping(TcpTransport& tcpTransport) {
		return SUCCESS;
	}
};

void releaseObject(ConnectionPool &cp) {
	std::cout << "Releasing objects in 2 sec" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(2));
	flag = 1;
	std::cout << "Releasing objects" << std::endl;
	cp.returnObject(addr, arrayOfT[0]);
	cp.returnObject(addr, arrayOfT[1]);
}

// END MOC


/* exitAfter is a basic timeout implementation */
void exitAfter(std::promise<void> &p, std::chrono::duration<int64_t> d,
		int &retVal) {
	auto status = p.get_future().wait_for(d);
	if (status != std::future_status::ready) {
		std::cerr << "FAIL: Timeout" << std::endl;
		exit(1);
	}
	retVal = retVal ? retVal : 0;
}

/** testWait tests the pool in WAIT configuration
 * the pool is exhausted and after 2 sec, two objects are release
 * and borrowed again to check that everything works
 */
void testWait() {
	std::cout << "Testing pool with WAIT configuration" << std::endl;
	ConfigurationBuilder builder;
	builder.connectionPool().exhaustedAction(WAIT).maxTotal(10);
	Configuration conf = builder.build();
	PoolTestObjectFactory* poolFactory = new PoolTestObjectFactory();
	std::shared_ptr<AbstractObjectFactory> factory(poolFactory);
	ConnectionPool cp(factory, conf.getConnectionPoolConfiguration());
	std::thread t1(releaseObject, std::ref(cp));
	cp.addObject(addr);
	flag = 0;
	while (true) {
		cp.borrowObject(addr);
		if (flag == 1) {
			cp.borrowObject(addr);
			break;
		}
	}
	t1.join();
	std::cout << "OK" << std::endl;
}

/** testExec tests the pool in EXECEPTION configuration
 * the pool is exhausted and after the exception one object is release
 * and borrowed again to check that everything works
 */
void testExec(int &retVal) {
	std::cout << "Testing pool with EXCEPTION configuration" << std::endl;
	ConfigurationBuilder builder;
	builder.connectionPool().exhaustedAction(EXCEPTION).maxTotal(10);
	Configuration conf = builder.build();
	PoolTestObjectFactory* poolFactory = new PoolTestObjectFactory();
	std::shared_ptr<AbstractObjectFactory> factory(poolFactory);
	ConnectionPool cp(factory, conf.getConnectionPoolConfiguration());
	cp.addObject(addr);
	try {
		while (true) {
			cp.borrowObject(addr);
		}
	} catch (NoSuchElementException &ex) {
		cp.returnObject(addr, arrayOfT[2]);
		try {
			cp.borrowObject(addr);
			retVal = 0;
			std::cout << "OK" << std::endl;
		} catch (Exception&) {
			retVal = 1;
			std::cerr << "FAIL: cannot get Obj after return" << std::endl;
		}
	} catch (Exception&) {
		retVal = 1;
		std::cerr << "FAIL: unexpected exception" << std::endl;
	}
}

int main(int argc, char** argv) {
	std::promise<void> promiseTimeout;
	int retVal;
	std::thread t1(exitAfter, std::ref(promiseTimeout),
			(std::chrono::seconds(20)), std::ref(retVal));
	testWait();
	testExec(retVal);
	promiseTimeout.set_value();
	t1.join();
	return retVal;
}

