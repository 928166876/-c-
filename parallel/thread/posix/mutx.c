#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <error.h>
#include <unistd.h>

#define BUFSIZE 1024
#define THRNUM  20
#define FNAME   "/tmp/out"
static pthread_mutex_t mutx = PTHREAD_MUTEX_INITIALIZER;

static void *add_fun(void *p){
    FILE *fp;
    char buf[BUFSIZE];
    fp = fopen(FNAME, "r+");
    if(fp == NULL){
        perror("fopen()");
        exit(1);
    }
    pthread_mutex_lock(&mutx);
    fgets(buf, BUFSIZE, fp);
    fseek(fp, 0, SEEK_SET);
    //sleep(1);
    fprintf(fp,"%d\n", atoi(buf)+1);
    fclose(fp);
    pthread_mutex_unlock(&mutx);

    pthread_exit(NULL);
}

int main(void){
    pthread_t tid[THRNUM];
    int err;
    for(int i = 0; i < THRNUM; i++){

        err = pthread_create(tid+i, NULL, add_fun, NULL);
        if(err){
            fprintf(stderr, "Pthread_create(): %s", strerror(err));
            exit(1);
        }
    }

    for(int i = 0; i < THRNUM; i++){
        pthread_join(tid[i], NULL);
    }

    exit(0);
}