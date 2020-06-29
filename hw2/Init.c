#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"
#include "hw2.h"
void Init(void) {
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        pThreadTbEnt[i].bUsed = 0;
        pCurrentThread = NULL;
    }
    pWaitingQueueHead = NULL;
    pWaitingQueueTail = NULL;
    for (int i = 0; i < MAX_READYQUEUE_NUM; i++) {
        pReadyQueueEnt[i].pHead = NULL;
        pReadyQueueEnt[i].pTail = NULL;
        pReadyQueueEnt[i].queueCount = 0;
    }
}
