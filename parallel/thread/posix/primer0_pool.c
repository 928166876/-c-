#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sched.h>

#define LEFT 30000000
#define RIGHT 30000200
#define THRNUM 3

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int num = 0;

static void *thr_handler(void *p);

int main(void){
    int err;
    pthread_t tid[THRNUM];

    for(int i = 0; i < THRNUM; i++){
        err = pthread_create(tid+i, NULL, thr_handler, (void *)i);
        if(err){
            fprintf(stderr, "pthread_create():%s", strerror(err));
            exit(1);
        }
    }

    for(int i = LEFT; i <= RIGHT; i++){
        pthread_mutex_lock(&mutex);
        while(num != 0){
            pthread_mutex_unlock(&mutex);
            sched_yield();
            pthread_mutex_lock(&mutex);
        }
        num = i;
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    while(num != 0){
        pthread_mutex_unlock(&mutex);
        sched_yield();
        pthread_mutex_lock(&mutex);
    }
    num = -1;
    pthread_mutex_unlock(&mutex);


    for(int i = 0; i < THRNUM; i++){
        pthread_join(tid[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    exit(0);

}



static void *thr_handler(void *p){
    int i = 0;
    while(1){
        pthread_mutex_lock(&mutex);
        while(num == 0){
            pthread_mutex_unlock(&mutex);
            sched_yield();
            pthread_mutex_lock(&mutex);
        }
        
        if(num == -1){
            pthread_mutex_unlock(&mutex);
            break;
        }

        i = num;
        num = 0;
        pthread_mutex_unlock(&mutex);
        int mark = 1;
        for(int j = 2; j < i/2; j++){
            if(i%j == 0){
                mark = 0;
                break;
            }
        }
        if(mark){
            printf("[%d] %d is a primer.\n", (int)p, i);
        }
    }
    pthread_exit(NULL);
}