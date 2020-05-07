#ifndef __THREAD_H__
#define __THREAD_H__

#define _GNU_SOURCE
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#define STACK_SIZE 1024 * 64
#define HASH_TBL_SIZE (8)
#define MAX_OBJLIST_NUM (3)
#define OBJ_INVALID (-1)

#define TIMESLICE (2)
#define MAX_READYQUEUE_NUM (8)
#define MAX_THREAD_NUM (64)

typedef int BOOL;
typedef int thread_t;
typedef struct _Thread Thread;
typedef void thread_attr_t;

typedef enum {
    THREAD_STATUS_RUN = 0,
    THREAD_STATUS_READY = 1,
    THREAD_STATUS_WAIT = 2,
    THREAD_STATUS_ZOMBIE = 3,
} ThreadStatus;

typedef struct _Thread {
    int stackSize;
    void *stackAddr;
    ThreadStatus status;
    int exitCode;
    pid_t pid;
    int priority;
    Thread *phNext;
    Thread *phPrev;
} Thread;

typedef struct _ReadyQueueEnt {
    int queueCount;
    Thread *pHead;
    Thread *pTail;
} ReadyQueueEnt;

typedef struct _ThreadTblEnt {
    BOOL bUsed;
    Thread *pThread;
} ThreadTblEnt;

/* head and tail pointers for ready queue */
ReadyQueueEnt pReadyQueueEnt[MAX_READYQUEUE_NUM];

/* head and tail pointers for waiting queue */
Thread *pWaitingQueueHead;
Thread *pWaitingQueueTail;
ThreadTblEnt pThreadTbEnt[MAX_THREAD_NUM];

int thread_create(thread_t *thread, thread_attr_t *attr, int priority,
                  void *(*start_routine)(void *), void *arg);
int thread_suspend(thread_t tid);
int thread_cancel(thread_t tid);
int thread_resume(thread_t tid);
thread_t thread_self();

void Init();
void InsertObjectToTail(Thread *pObj, int ObjNum);
void InsertObjectToHead(Thread *pObj, int objNum);
Thread *GetObjectByNum(int objnum);
Thread *GetObjectFromObjFreeList();
BOOL DeleteObject(Thread *pObj);
void InsertObjectIntoObjFreeList(Thread *pObj);
#endif /* __THREAD_H__ */
