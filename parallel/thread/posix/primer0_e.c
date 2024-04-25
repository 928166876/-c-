#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define LEFT 30000000
#define RIGHT 30000200
#define THRNUM (RIGHT-LEFT+1)


static void *thr_handler(void *p);

struct thr_arg_st{
    int n;
};


int main(void){
    int err;
    pthread_t tid[THRNUM];
    struct thr_arg_st *p;
    void *ptr;

    for(int i = LEFT; i <= RIGHT; i++){
        p = malloc(sizeof(*p));
        if(p == NULL){
            perror("malloc()");
            exit(1);
        }
        p->n = i;
        err = pthread_create((tid+i-LEFT), NULL, thr_handler, p);
        if(err){
            fprintf(stderr, "pthread_create():%s", strerror(err));
            exit(1);
        }
    }

    for(int i = LEFT; i <= RIGHT; i++){
        pthread_join(tid[i-LEFT], &ptr);
        free(ptr);
    }

    exit(0);

}



static void *thr_handler(void *p){

    int i = ((struct thr_arg_st*)p)->n;
    //free(p);
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

    pthread_exit(p);
}