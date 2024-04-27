#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <string.h>

#include "mysem.h"

#define LEFT 30000000
#define RIGHT 30000200
#define THRNUM (RIGHT-LEFT+1)
#define N 5

static void *thr_handler(void *p);
static mysem_t *sem;



int main(void){
    int err;
    pthread_t tid[THRNUM];

    sem = mysem_init(N);
    if(sem == NULL){
        fprintf(stderr, "mysem_init() faild.\n");
        exit(1);
    }

    for(int i = LEFT; i <= RIGHT; i++){
        mysem_del(sem, 1);
        err = pthread_create((tid+i-LEFT), NULL, thr_handler, (void *)i);
        if(err){
            fprintf(stderr, "pthread_create():%s", strerror(err));
            exit(1);
        }
    }

    for(int i = LEFT; i <= RIGHT; i++){
        pthread_join(tid[i-LEFT], NULL);
    }

    mysem_destory(sem);

    exit(0);

}



static void *thr_handler(void *p){

    int i = (int)p;
    int mark = 1;
    for(int j = 2; j < i/2; j++){
        if(i%j == 0){
            mark = 0;
            break;
        }
    }
    if(mark){
        printf("%d is a primer.\n", i);
    }
    sleep(5);
    mysem_add(sem, 1);
    pthread_exit(NULL);
}