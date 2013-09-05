#ifndef ISPN_HOTROD_WINDOWS_RUNONCE_H
#define ISPN_HOTROD_WINDOWS_RUNONCE_H

#include <windows.h>

namespace infinispan {
namespace hotrod {
namespace sys {

class RunOnce {
  public:
    RunOnce(void(*fn)()) : init_routine(fn) {
        InitOnceInitialize(&initOnce);
    }

    inline void runOnce();

  private:
    INIT_ONCE initOnce;
    void (*init_routine)(void);
    static BOOL CALLBACK onceRelay (PINIT_ONCE pio, PVOID param, PVOID *context) {
        RunOnce *r = (RunOnce *) param;
        void (*fn)() = r->init_routine;
        (*fn)();
        return true;
    }
        
    // non copyable:
    RunOnce(RunOnce const &);
    RunOnce& operator=(RunOnce const &);
};

void RunOnce::runOnce() {
    InitOnceExecuteOnce(&initOnce, &RunOnce::onceRelay, (void *) this, NULL);
}

}}}
#endif  /* ISPN_HOTROD_WINDOWS_RUNONCE_H */
