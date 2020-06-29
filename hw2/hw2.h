#ifndef _HW2_H_
#define _HW2_H_
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "Thread.h"
void InsertReadyQueueToTail(Thread *thread, int priority);
Thread *GetThreadFromReadyqueueHead(int priority);
void InsertObjectToTail(Thread *pObj, int ObjNum);
Thread *GetObjectByNum(thread_t tid);
Thread *GetObjectFromObjFreeList();
BOOL DeleteObject(Thread *pObj);
Thread *GetThreadFromWaitingqueue(Thread *thread);//
void InsertObjectIntoObjFreeList(Thread *pObj);
#endif
