#include "Init.h"
#include "Scheduler.h"
//#include "TestCase1.h"
#include "Thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#define STACK_SIZE 1024 * 64
/*
void *threadFunc2(void *arg);
void *threadFunc1(void *arg) {
    thread_t tid;
    int max = *((int *)arg);
    int i;
    for (int i = 0; i < 5; i++) {
        printf("threadFunc1 %d\n", getpid());
        thread_create(&tid, NULL, 0, threadFunc2, &arg);
        waitpid(pThreadTbEnt[tid].pThread->pid, NULL, 0);
    }
}
void *threadFunc2(void *arg) {
    int max = *((int *)arg);
    int i;
    printf("threadFunc2 %d\n", i);
}

int main() {
    thread_t tid;
    int arg = 100;
    Init();
    printf("thread start2\n");
    thread_create(&tid, NULL, 0, threadFunc1, &arg);
    // printf("tid : %d", tid);
    waitpid(pThreadTbEnt[tid].pThread->pid, NULL, 0);
    int i = 0;
    while (1) {
        if (pThreadTbEnt[i].bUsed == 1) {
            printf("%d\n", i);
            i++;
        } else {
            break;
        }
    }
}
*/
#include "TestCase2.h"
int main() {
    thread_t tid;
    int arg = 100;
    Init();
    printf("thread start2\n");
    thread_create(&tid, NULL, 0, (void *)TestCase2, 0);
    // printf("tid : %d", tid);
    RunScheduler();

    while (1) {
    }
    // while (1) {
    //  if (pThreadTbEnt[i].bUsed == 1) {
    //    printf("%d\n", i);
    //   i++;
    //} else {
    //  break;
    // }
    // }
}
