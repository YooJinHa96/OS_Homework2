#ifndef __HW1_H__
#define __HW1_H__
#include "Thread.h"
#include <stdio.h>
#define HASH_TBL_SIZE (8)
#define MAX_OBJLIST_NUM (3)
#define OBJ_INVALID (-1)

typedef struct Thread thread;
typedef int BOOL;

struct Thread {
    int objnum;
    Thread *phPrev;
    Thread *phNext;
};

typedef struct __ReadyQueueEnt {
    int queueCount;
    Thread *pHead;
    Thread *pTail;
} ReadyQueueEnt;

ReadyQueueEnt pReadyQueueEnt[HASH_TBL_SIZE];

Thread *pWaitingQueueHead;
Thread *pWaitingQueueTail;

void Init();
void InsertObjectToTail(Thread *pObj, int ObjNum);
void InsertObjectToHead(Thread *pObj, int objNum);
Thread *GetObjectByNum(int objnum);
Thread *GetObjectFromObjFreeList();
BOOL DeleteObject(Thread *pObj);
void InsertObjectIntoObjFreeList(Thread *pObj);

#endif
