#include <hotrod/sys/Runnable.h>
#include <hotrod/sys/Thread.h>

#include <iostream>
#include <string>

int passFail = 0;

using namespace infinispan::hotrod::sys;

struct Sleeper : public Runnable {
  public:
    Sleeper(std::string name, unsigned len, int &s) : me(name), sleepTime(len), status(s) {}
    void run() {
	Thread::sleep(sleepTime);
	status = 1;
	std::cout << me << " done" << std::endl;
    }
    ~Sleeper() {}
    std::string me;
    unsigned sleepTime;
    int& status;
};
    

void threadTest() {
    int st1 = 0;
    int st2 = 0;
    std::cout << "starting threads" << std::endl;
    Sleeper s1("sleeper 1", 500, st1);
    Sleeper s2("sleeper 2", 1500, st2);
    Thread t1((Runnable &) s1);
    Thread t2((Runnable &) s2);
    if ((st1 != 0) || (st2 != 0)) {
	passFail = 1;
	std::cerr << "initial sleep fail" << std::endl;
    }
    t1.join();
    t2.join();
    if ((st1 != 1) || (st2 != 1)) {
	passFail = 1;
	std::cerr << "post yield status fail (" << st1 << ") (" << st2 << ")" << std::endl;
    }
    return;
}

int main(int, char**) {
    threadTest();
    return passFail;
}
        
