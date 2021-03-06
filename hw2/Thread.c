#include "Init.h"
#include "Scheduler.h"
#include "hw2.h"
#include "Thread.h"

void InsertReadyQueueToTail(Thread *thread, int priority)
{
    int hashKey = priority;
    thread->priority = priority;
    if (pReadyQueueEnt[hashKey].pTail != NULL)
    {
        Thread *o = pReadyQueueEnt[hashKey].pTail;
        pReadyQueueEnt[hashKey].pTail = thread;
        thread->phNext = NULL;
        thread->phPrev = o;
        o->phNext = thread;
    }
    else
    {
        pReadyQueueEnt[hashKey].pHead = thread;
        pReadyQueueEnt[hashKey].pTail = thread;
        thread->phNext = NULL;
        thread->phPrev = NULL;
    }
    pReadyQueueEnt[hashKey].queueCount++;
}


Thread *GetObjectByNum(thread_t tid)
{
    int hashKey = pThreadTbEnt[tid].pThread->priority;
    pid_t pid = pThreadTbEnt[tid].pThread->pid;
    Thread *o;
    for (o = pReadyQueueEnt[hashKey].pHead; o != NULL; o = o->phNext)
    {
        if (o->pid == pid)
        {
            return o;
        }
    }
    return NULL;
}
Thread *SelectThreadFromReadyqueueHead(int priority)
{

    int hashkey = priority;
    Thread *t = pReadyQueueEnt[hashkey].pHead;

    if (t != NULL)
    {
        return t;
    }
    else
    {
        return NULL;
    }
}
Thread *GetThreadFromReadyqueueHead(int priority)
{

    int hashkey = priority;
    Thread *t = pReadyQueueEnt[hashkey].pHead;
    if (DeleteObject(t) == 0)
    {
        printf("delete error\n");
    }

    if (t != NULL)
    {
        return t;
    }
    else
    {
        return NULL;
    }
}

Thread *GetThreadFromWaitingqueue(Thread *thread)
{
    Thread *o = pWaitingQueueHead;
    int count = 0;
    for (Thread *p = pWaitingQueueHead; p != NULL; p = p->phNext)
    {
        count++;
    }
    if (pWaitingQueueHead == NULL)
    {
        return NULL;
    }
    for (o; o != NULL; o = o->phNext)
    {
        if (o == thread)
        {
            if (count == 1)
            {
                pWaitingQueueHead = NULL;
                pWaitingQueueTail = NULL;
                return o;
            }
            else
            {
                if (o == pWaitingQueueHead)
                {
                    pWaitingQueueHead = o->phNext;
                }
                else if (o == pWaitingQueueTail)
                {
                    pWaitingQueueTail = o->phPrev;
                }
                else
                {
                    o->phNext->phPrev = o->phPrev;
                    o->phPrev->phNext = o->phNext;
                }
            }
            return o;
        }
    }

    return o;
}
BOOL DeleteObject(Thread *pObj)
{
    int hashkey = pObj->priority;
    Thread *o = pReadyQueueEnt[hashkey].pHead;
    if (pReadyQueueEnt[hashkey].queueCount == 0)
    {
        return 0;
    }
    else if (pReadyQueueEnt[hashkey].queueCount == 1)
    {
        if (o == pObj)
        {
            pReadyQueueEnt[hashkey].pHead = NULL;
            pReadyQueueEnt[hashkey].pTail = NULL;
            pReadyQueueEnt[hashkey].queueCount--;
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        for (o; o != NULL; o = o->phNext)
        {
            if (o == pObj)
            {
                if (o->phPrev == NULL)
                {
                    pReadyQueueEnt[hashkey].pHead = o->phNext;
                    o->phNext->phPrev = NULL;
                }
                else if (o->phNext == NULL)
                {
                    pReadyQueueEnt[hashkey].pTail = o->phPrev;
                    o->phPrev->phNext = NULL;
                }
                else
                {
                    o->phPrev->phNext = o->phNext;
                    o->phNext->phPrev = o->phPrev;
                }
                pReadyQueueEnt[hashkey].queueCount--;
                return 1;
            }
        }
    }
    return 0;
}

//고칠 부분 ->  check
void InsertObjectIntoObjFreeList(Thread *pObj)
{
    if (pWaitingQueueTail == NULL)
    {
        pWaitingQueueHead = pObj;
        pWaitingQueueTail = pObj;
        pObj->phNext = NULL;
        pObj->phPrev = NULL;
    }
    else
    {
        Thread *o = pWaitingQueueTail;
        pWaitingQueueTail = pObj;
        pObj->phNext = NULL;
        pObj->phPrev = o;
        o->phNext = pObj;
    }
}
void sigchild_handler(int signum)
{

    Thread *waitthread = pWaitingQueueHead;
    if (waitthread != NULL)
    {
        for (; waitthread->pid != getpid(); waitthread = waitthread->phNext)
        {
        }
    }
    if (signum == SIGCHLD)
    {
        if (GetThreadFromWaitingqueue(waitthread) == NULL)
        {
            return;
        }
        InsertReadyQueueToTail(waitthread, waitthread->priority);
        waitthread->status = THREAD_STATUS_READY;
    }
}
int thread_create(thread_t *thread, thread_attr_t *attr, int priority,
                  void *(*start_routine)(void *), void *arg)
{
    pid_t pid;
    int flags = SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM;
    void *pStack;
    int pr = priority;
    pStack = malloc(STACK_SIZE);

    pid = clone((void *)start_routine, (void *)pStack + STACK_SIZE, flags, arg);
    kill(pid, SIGSTOP);
    struct sigaction act;
    act.sa_handler = sigchild_handler;
    act.sa_flags = SA_NOCLDSTOP;

    sigaction(SIGCHLD, &act, NULL);
    for (int i = 0; i < MAX_THREAD_NUM; i++)
    {
        if (!pThreadTbEnt[i].bUsed)
        {
            *thread = i;
            pThreadTbEnt[i].bUsed = 1;
            pThreadTbEnt[i].pThread = malloc(sizeof(Thread));
            pThreadTbEnt[i].pThread->pid = pid;
            pThreadTbEnt[i].pThread->priority = priority;
            break;
        }
    }

    if (pCurrentThread == NULL)
    { // Testcase Thread create
        InsertReadyQueueToTail(pThreadTbEnt[*thread].pThread, priority);
        pThreadTbEnt[*thread].pThread->status = THREAD_STATUS_READY;
    }
    else
    {
        if (pCurrentThread->priority <=
            pThreadTbEnt[*thread].pThread->priority)
        {
            InsertReadyQueueToTail(pThreadTbEnt[*thread].pThread, priority);
            pThreadTbEnt[*thread].pThread->status = THREAD_STATUS_READY;
        }
        else
        {
            InsertReadyQueueToTail(pCurrentThread, pCurrentThread->priority);
            pCurrentThread->status = THREAD_STATUS_READY;
            pThreadTbEnt[*thread].pThread->status = THREAD_STATUS_RUN;
            __ContextSwitch(pCurrentThread->pid,
                            pThreadTbEnt[*thread].pThread->pid);
            pCurrentThread = pThreadTbEnt[*thread].pThread;
        }
    }
    return *thread;
}
// waiting일때도 알아본다
int thread_suspend(thread_t tid)
{

    Thread *thread = GetObjectByNum(tid);
    if (thread == NULL)
    {
        return -1;
    }
    else if (thread->status == THREAD_STATUS_READY)
    {
        DeleteObject(thread);
        pThreadTbEnt[tid].pThread->status = THREAD_STATUS_WAIT;
        InsertObjectIntoObjFreeList(thread);
        return 0;
    }
    else if (thread->status == THREAD_STATUS_WAIT)
    {
        return 0;
    }
}

int thread_cancel(thread_t tid)
{
    if (!pThreadTbEnt[tid].bUsed)
    { // error
        return -1;
    }
    kill(pThreadTbEnt[tid].pThread->pid, SIGKILL);
    switch (pThreadTbEnt[tid].pThread->status)
    {
    case THREAD_STATUS_READY:
    {
        DeleteObject(pThreadTbEnt[tid].pThread);
        pThreadTbEnt[tid].bUsed = 0;
        free(pThreadTbEnt[tid].pThread);
        break;
    }
    case THREAD_STATUS_WAIT:
    {
        GetThreadFromWaitingqueue(pThreadTbEnt[tid].pThread);
        pThreadTbEnt[tid].bUsed = 0;
        free(pThreadTbEnt[tid].pThread);
        break;
    }

    default:
        break;
    }
    return 0;
}

int thread_resume(thread_t tid)
{
    if (pThreadTbEnt[tid].bUsed == 0)
    {
        return -1;
    }

    int taPr = pThreadTbEnt[tid].pThread->priority;
    if (pCurrentThread->priority <= taPr)
    {
        pThreadTbEnt[tid].pThread->status = THREAD_STATUS_READY;
        if (GetThreadFromWaitingqueue(pThreadTbEnt[tid].pThread) == NULL)
        {
            return -1;
        }

        InsertReadyQueueToTail(pThreadTbEnt[tid].pThread, taPr);
    }
    else
    {
        InsertReadyQueueToTail(pCurrentThread, pCurrentThread->priority);
        pCurrentThread->status = THREAD_STATUS_READY;
        pThreadTbEnt[tid].pThread->status = THREAD_STATUS_RUN;
        if (GetThreadFromWaitingqueue(pThreadTbEnt[tid].pThread) == NULL)
        {
            return -1;
        }
        __ContextSwitch(pCurrentThread->pid, pThreadTbEnt[tid].pThread->pid);
        pCurrentThread = pThreadTbEnt[tid].pThread;
    }
    return 0;
}

thread_t thread_self()
{
    pid_t pid = getpid();
    thread_t tid;
    for (int i = 0; MAX_THREAD_NUM; i++)
    {
        if (pThreadTbEnt[i].pThread->pid == pid)
        {
            tid = i;
            return tid;
        }
    }
}
int thread_exit(void *retval)
{

    Thread *thread = pCurrentThread;
    thread->exitCode = *(int *)retval;

    Thread *newThread;

    InsertObjectIntoObjFreeList(thread);

    thread->status = THREAD_STATUS_ZOMBIE;

    for (int i = 0; i < MAX_READYQUEUE_NUM; i++)
    {
        if (pReadyQueueEnt[i].queueCount != 0)
        {

            newThread = GetThreadFromReadyqueueHead(i);

            DeleteObject(newThread);

            newThread->status = THREAD_STATUS_RUN;
            __ContextSwitch(0, newThread->pid);
            pCurrentThread = newThread;
            exit(1);
            break;
        }
    }
    return -1;
}
pid_t ppid = 1;

int thread_join(thread_t tid, void **retval)
{ // stack ??

    Thread *newthread;
    Thread *prethread = pCurrentThread;

    if (pThreadTbEnt[tid].bUsed == 0)
    {
        return -1;
    }
    if (pThreadTbEnt[tid].pThread->status == THREAD_STATUS_ZOMBIE)
    {

        *(int *)*retval = pThreadTbEnt[tid].pThread->exitCode;
        GetThreadFromWaitingqueue(pThreadTbEnt[tid].pThread);
        pThreadTbEnt[tid].bUsed = 0;
        free(pThreadTbEnt[tid].pThread);
        return 0;
    }
    else
    {
        pCurrentThread->status = THREAD_STATUS_WAIT;
        for (int i = 0; i < MAX_READYQUEUE_NUM; i++)
        {
            if (pReadyQueueEnt[i].queueCount != 0)
            {
                //newthread = GetThreadFromReadyqueueHead(i);
                newthread = SelectThreadFromReadyqueueHead(i);
                break;
            }
        }
        DeleteObject(pCurrentThread);
        InsertObjectIntoObjFreeList(pCurrentThread);

        newthread->status = THREAD_STATUS_RUN;
        //__ContextSwitch(pCurrentThead->pid, newthread->pid);
        //pCurrentThead = newthread;
        ppid = pThreadTbEnt[tid].pThread->pid;

        pause();
        int *rv = (int *)malloc(sizeof(int));
        *rv = pThreadTbEnt[tid].pThread->exitCode;
        *retval = rv;

        GetThreadFromWaitingqueue(pThreadTbEnt[tid].pThread);

        pThreadTbEnt[tid].bUsed = 0;

        free(pThreadTbEnt[tid].pThread);

        return 0;
    }
}
