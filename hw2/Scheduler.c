#include "Scheduler.h"
#include "Init.h"
#include "Thread.h"
#include "hw2.h"
void sig_handler(int sign);
void sig_handler(int sign)
{
    if (sign == SIGALRM)
    {
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

            while (1)
                ;
        }
        else if (pCurrentThread == NULL)
        { // thread test case start
            pCurrentThread = GetThreadFromReadyqueueHead(0);
            if (pCurrentThread != NULL)
            {
                kill(pCurrentThread->pid, SIGCONT);
            } // Assume Testcase prirority = 0
            else if (pCurrentThread == NULL)
            {
                while (1)
                    ;
            }
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
                    InsertReadyQueueToTail(pCurrentThread, pCurrentThread->priority);
                    pCurrentThread->status = THREAD_STATUS_READY;
                    __ContextSwitch(pCurrentThread->pid, newThread->pid);
                    pCurrentThread = newThread;
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
    }
    else if (pCurrentThread == NULL)
    { // thread test case start
        pCurrentThread = GetThreadFromReadyqueueHead(0);
        if (pCurrentThread != NULL)
        {
            kill(pCurrentThread->pid, SIGCONT);
        } // Assume Testcase prirority = 0
        else if (pCurrentThread == NULL)
        {
            while (1)
                ;
        }
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
                InsertReadyQueueToTail(pCurrentThread, pCurrentThread->priority);
                pCurrentThread->status = THREAD_STATUS_READY;
                __ContextSwitch(pCurrentThread->pid, newThread->pid);
                pCurrentThread = newThread;
                break;
            }
        }
    }
    alarm(TIMESLICE);
}

void __ContextSwitch(int curpid, int newpid)
{
    if (curpid != 0)
    {
        kill(curpid, SIGSTOP);
    }
    kill(newpid, SIGCONT);
}
