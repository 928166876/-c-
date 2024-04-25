#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

static void cleanup(void *p){
    puts(p);
}

static void *func(void *p){
    puts("thread is working!");

    pthread_cleanup_push(cleanup, "1");
    pthread_cleanup_push(cleanup, "2");
    pthread_cleanup_push(cleanup, "3");


    //pthread_exit(NULL);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);

    puts("push over!");
    pthread_exit(NULL);
}


int main(void){
    int err;
    puts("Begin!");
    pthread_t tid;

    err =  pthread_create(&tid, NULL, func, NULL);
    if(err){
        fprintf(stderr, "pthread_creat err %s\n", strerror(err));
        exit(1);
    }

    pthread_join(tid, NULL);
    puts("End!");
    exit(0);
}