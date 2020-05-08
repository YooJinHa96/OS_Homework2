#include "Scheduler.h"
#include "Init.h"
#include "Thread.h"

int RunScheduler(void) {

    // Readyqueue empty check
    for (int i = 0; i < MAX_READYQUEUE_NUM; i++) {
        if (pReadyQueueEnt[i].queueCount) {
        }
    }
}

void __ContextSwitch(int curpid, int newpid) {

    kill(curpid, SIGSTOP);
    kill(newpid, SIGCONT);
}
