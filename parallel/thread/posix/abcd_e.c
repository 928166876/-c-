#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <error.h>
#include <unistd.h>

#define THRNUM  4

static pthread_mutex_t mutx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int num = 0;

static int next(int n){
    if(n + 1 == THRNUM)
        return 0;
    return n + 1;
}

static void *add_func(void *p){
    int n = (int)p;
    int ch = 'a' + n;
    while(1){
        pthread_mutex_lock(&mutx);
        while(num != n){
            pthread_cond_wait(&cond, &mutx);
        }
        write(1, &ch, 1);
        num = next(num);
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutx);
    }

    pthread_exit(NULL);
}


int main(void){
    int err;
    pthread_t tid[THRNUM];

    for(int i = 0; i < THRNUM; i++){


        err = pthread_create(tid+i, NULL, add_func, (void *)i);
        if(err){
            fprintf(stderr, "Pthread_create(): %s", strerror(err));
            exit(1);
        }
    }


    alarm(5);

    for(int i = 0; i < THRNUM; i++){
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mutx);
    pthread_cond_destroy(&cond);
    exit(0);
}