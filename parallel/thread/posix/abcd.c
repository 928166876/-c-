#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <error.h>
#include <unistd.h>

#define THRNUM  4

static pthread_mutex_t mutx[THRNUM];

static int next(int n){
    if(n + 1 == THRNUM)
        return 0;
    return n + 1;
}

static void *add_func(void *p){
    int n = (int)p;
    int ch = 'a' + n;
    while(1){
        pthread_mutex_lock(mutx+n);
        write(1, &ch, 1);
        pthread_mutex_unlock(mutx + next(n));
    }


    pthread_exit(NULL);
}


int main(void){
    int err;
    pthread_t tid[THRNUM];

    for(int i = 0; i < THRNUM; i++){

        pthread_mutex_init(mutx+i, NULL);
        pthread_mutex_lock(mutx+i);
        err = pthread_create(tid+i, NULL, add_func, (void *)i);
        if(err){
            fprintf(stderr, "Pthread_create(): %s", strerror(err));
            exit(1);
        }
    }
    pthread_mutex_unlock(mutx+0);

    alarm(5);

    for(int i = 0; i < THRNUM; i++){
        pthread_join(tid[i], NULL);
    }

    exit(0);
}