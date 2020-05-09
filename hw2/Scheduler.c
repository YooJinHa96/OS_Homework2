#include "Scheduler.h"
#include "Init.h"
#include "Thread.h"
void sig_handler(int sign)
{
    if (sign == SIGALRM)
    {
        alarm(TIMESLICE);
        int count = 0;
        int t_priority;
        Thread *newThread;
        // Readyqueue empty check
        for (int i = 0; i < MAX_READYQUEUE_NUM; i++)
        {
            if (pReadyQueueEnt[i].queueCount == 0)
            {
                count++;
            }
        }
        if (count == MAX_READYQUEUE_NUM)
        {
        }
        else if (count < MAX_READYQUEUE_NUM)
        {

            for (int i = 0; i < MAX_READYQUEUE_NUM; i++)
            {
                if (pReadyQueueEnt[i].queueCount != 0)
                {
                    t_priority = i;
                    newThread = GetThreadFromReadyqueueHead(t_priority);
                    newThread->status = THREAD_STATUS_RUN;
                    InsertReadyQueueToTail(pCurrentThead, pCurrentThead->priority);
                    pCurrentThead->status = THREAD_STATUS_READY;
                    __ContextSwitch(pCurrentThead->pid, newThread->pid);
                    pCurrentThead = newThread;
                    break;
                }
            }
        }
        alarm(TIMESLICE);
    }
}
int RunScheduler(void)
{

    int count = 0;
    int t_priority;
    Thread *newThread;
    signal(SIGALRM, sig_handler);

    // Readyqueue empty check
    for (int i = 0; i < MAX_READYQUEUE_NUM; i++)
    {
        if (pReadyQueueEnt[i].queueCount == 0)
        {
            count++;
        }
    }
    if (count == MAX_READYQUEUE_NUM)
    {
        // alarm(TIMESLICE);
    }
    else if (pCurrentThead == NULL)
    { // thread test case start
        pCurrentThead =
            GetThreadFromReadyqueueHead(0); // Assume Testcase prirority = 0
        kill(pCurrentThead->pid, SIGCONT);
        // alarm(TIMESLICE);
    }
    else if (count < MAX_READYQUEUE_NUM)
    {

        for (int i = 0; i < MAX_READYQUEUE_NUM; i++)
        {
            if (pReadyQueueEnt[i].queueCount != 0)
            {
                t_priority = i;
                newThread = GetThreadFromReadyqueueHead(t_priority);
                newThread->status = THREAD_STATUS_RUN;
                InsertReadyQueueToTail(pCurrentThead, pCurrentThead->priority);
                pCurrentThead->status=THREAD_STATUS_READY;
                __ContextSwitch(pCurrentThead->pid, newThread->pid);
                pCurrentThead = newThread;
                break;
            }
        }
    }
    alarm(TIMESLICE);
}

void __ContextSwitch(int curpid, int newpid)
{
    kill(curpid, SIGSTOP);
    kill(newpid, SIGCONT);
    // pCurrentThead = pThreadTbEnt[newpid].pThread;
}
