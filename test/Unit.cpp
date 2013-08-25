#include <hotrod/sys/Runnable.h>
#include <hotrod/sys/Thread.h>
#include <hotrod/sys/Mutex.h>
#include <hotrod/sys/Condition.h>

#include <iostream>
#include <string>

volatile int passFail = 0;
volatile int available = 0;
volatile bool waitersDone = false;

using namespace infinispan::hotrod::sys;

class Sleeper : public Runnable {
  public:
    Sleeper(std::string name, unsigned len, int &s) : me(name), sleepTime(len), status(s) {}
    void run() {
        Thread::sleep(sleepTime);
        status = 1;
        std::cout << me << " done" << std::endl;
    }
  private:
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

class Waiter : public Runnable {
  public:
    Waiter(std::string name, Mutex& m, Condition &c, bool b) : me(name), lock(m), condition(c), timed(b)  {}
    void run() {
        if (timed) {
            ScopedLock<Mutex> l(lock);
            passFail = 5;
            // test is expected to timeout
            if (condition.wait(lock, 2000000)) {
                std::cout << me << " timed waiter fail" << std::endl;
                return;
            }
            passFail = 0;
            std::cout << me << " timed waiter pass" << std::endl;
            return;
        }

        ScopedLock<Mutex> l(lock);
        while(1) {
            if (waitersDone)
                return;
            if (available > 0) {
                available--;
                std::cout << me << " consumed one" << std::endl;
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
    std::cout << "starting sync threads" << std::endl;
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
            std::cerr << "single notify fail" << std::endl;
            return;
        }
        std::cout << "single notify pass" << std::endl;
        available = 5;
        condition.notifyAll();
    }

    Thread::sleep(1000);
    {
        ScopedLock<Mutex> l(lock);
        if (available != 0) {
            passFail = 1;
            std::cerr << "notifyAll fail" << std::endl;
            return;
        }
        waitersDone = true;
        condition.notifyAll();
    }

    t1.join();
    t2.join();
    std::cout << "Sync test passed" << std::endl;
    return;
}

int main(int, char**) {
    threadTest();
    syncTest();
    return passFail;
}
