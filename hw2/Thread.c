#include "Init.h"
#include "Scheduler.h"

#include "Thread.h"

/*
void Init() {

}
*/
void InsertObjectToTail(Thread *pObj, int ObjNum)
{
    int hashKey = ObjNum % HASH_TBL_SIZE;
    pObj->priority = ObjNum;
    if (pReadyQueueEnt[hashKey].pTail != NULL)
    {
        Thread *o = pReadyQueueEnt[hashKey].pTail;
        pReadyQueueEnt[hashKey].pTail = pObj;
        pObj->phNext = NULL;
        pObj->phPrev = o;
        o->phNext = pObj;
    }
    else
    {
        pReadyQueueEnt[hashKey].pHead = pObj;
        pReadyQueueEnt[hashKey].pTail = pObj;
        pObj->phNext = NULL;
        pObj->phPrev = NULL;
    }
    pReadyQueueEnt[hashKey].queueCount++;
}

void InsertObjectToHead(Thread *pObj, int objNum)
{
    int hashKey = objNum % HASH_TBL_SIZE;
    pObj->priority = objNum;
    if (pReadyQueueEnt[hashKey].pHead != NULL)
    {
        Thread *o = pReadyQueueEnt[hashKey].pHead;
        pReadyQueueEnt[hashKey].pHead = pObj;
        pObj->phNext = o;
        pObj->phPrev = NULL;
        o->phPrev = pObj;
    }
    else
    {
        pReadyQueueEnt[hashKey].pHead = pObj;
        pReadyQueueEnt[hashKey].pTail = pObj;
        pObj->phNext = NULL;
        pObj->phPrev = NULL;
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

Thread *GetObjectFromObjFreeList()
{
    Thread *o = pWaitingQueueTail;

    if (pWaitingQueueTail == NULL)
    {
        return NULL;
    }
    else if (o->phPrev == NULL)
    {
        pWaitingQueueTail = NULL;
        pWaitingQueueTail = NULL;
    }
    else
    {
        pWaitingQueueTail = o->phPrev;
        o->phPrev->phNext = NULL;
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

//고칠 부분
void InsertObjectIntoObjFreeList(Thread *pObj)
{
    pObj->priority = OBJ_INVALID;
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

int thread_create(thread_t *thread, thread_attr_t *attr, int priority,
                  void *(*start_routine)(void *), void *arg)
{
    pid_t pid;
    int flags = SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM;
    void *pStack;
    int pr = priority;
    pStack = malloc(STACK_SIZE);

    pid = clone((void *)start_routine, (void *)pStack + STACK_SIZE, flags, arg);
    printf("thread id : %d\n", pid);
    // kill(pid, SIGSTOP);
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
    /*
        if (pCurrentThead == NULL) {
            kill(pid, SIGCONT);
            pCurrentThead = pThreadTbEnt[*thread].pThread;
            pThreadTbEnt[*thread].pThread->status = THREAD_STATUS_RUN;
        } else {
            if (pCurrentThead->priority <
       pThreadTbEnt[*thread].pThread->priority) {
                InsertObjectToTail(pThreadTbEnt[*thread].pThread, priority);
                pThreadTbEnt[*thread].pThread->status = THREAD_STATUS_READY;
            }
        }
        */
}

int thread_suspend(thread_t tid)
{

    Thread *thread = GetObjectByNum(tid);
    if (thread == NULL)
    {
        return -1;
    }
    else
    {
        DeleteObject(thread);
        pThreadTbEnt[tid].pThread->status = THREAD_STATUS_WAIT;
        InsertObjectIntoObjFreeList(thread);
        return 0;
    }
}

int thread_cancel(thread_t tid) {}

int thread_resume(thread_t tid) {}

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
