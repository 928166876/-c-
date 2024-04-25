#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

static void *func(void *p){
    puts("thread is working!");
    return NULL;
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
    sleep(1);
    puts("End!");
    exit(0);
}