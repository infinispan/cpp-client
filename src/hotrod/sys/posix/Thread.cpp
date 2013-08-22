#include "hotrod/sys/Thread.h"
#include "hotrod/sys/Runnable.h"
#include "hotrod/sys/posix/check.h"

#include <pthread.h>
#include <unistd.h>
#include <time.h>

namespace infinispan {
namespace hotrod {
namespace sys {

namespace {
void* runRunnable(void* p)
{
    static_cast<Runnable*>(p)->run();
    return 0;
}
}

class ThreadPrivate {
public:
    pthread_t thread;

    ThreadPrivate(Runnable* runnable) {
        HOTROD_POSIX_THROW_IF(::pthread_create(&thread, NULL, runRunnable, runnable));
    }

    ThreadPrivate() : thread(::pthread_self()) {}
};

Thread::Thread() {}

Thread::Thread(Runnable* runnable) : impl(new ThreadPrivate(runnable)) {}

Thread::Thread(Runnable& runnable) : impl(new ThreadPrivate(&runnable)) {}

Thread::operator bool() {
    return impl;
}

bool Thread::operator==(const Thread& t) const {
    return ::pthread_equal(impl->thread, t.impl->thread) != 0;
}

bool Thread::operator!=(const Thread& t) const {
    return !(*this==t);
}

void Thread::join(){
    if (impl) {
        HOTROD_POSIX_THROW_IF(::pthread_join(impl->thread, 0));
    }
}

Thread Thread::current() {
    Thread t;
    t.impl.reset(new ThreadPrivate());
    return t;
}

void Thread::sleep(long millis) {
    struct timespec ts;
    if (millis < 1) millis = 1;
    ts.tv_nsec = (millis % 1000) * 1000000;
    ts.tv_sec = millis / 1000;
    ::nanosleep(&ts, NULL);
    return;
}

}}}
