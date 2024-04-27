#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "mysem.h"


struct mysem_st{
    int values;
    pthread_mutex_t mut;
    pthread_cond_t cond;
};

mysem_t *mysem_init(int initval){
    struct mysem_st *me;
    me = malloc(sizeof(*me));
    if(me == NULL){
        return NULL;
    }

    me->values = initval;
    pthread_mutex_init(&me->mut, NULL);
    pthread_cond_init(&me->cond, NULL);

    return me;
}

int mysem_add(mysem_t *ptr, int n){
    struct mysem_st *me = ptr;

    pthread_mutex_lock(&me->mut);
    me->values += n;
    pthread_cond_broadcast(&me->cond);
    pthread_mutex_unlock(&me->mut);
    return n;

}

int mysem_del(mysem_t *ptr, int n){
    struct mysem_st *me = ptr;

    pthread_mutex_lock(&me->mut);
    while(me->values < n){
        pthread_cond_wait(&me->cond, &me->mut);
    }
    me->values -= n;
    pthread_mutex_unlock(&me->mut);

    return n;
}

int mysem_destory(mysem_t *ptr){
    struct mysem_st *me = ptr;

    pthread_mutex_destroy(&me->mut);
    pthread_cond_destroy(&me->cond);
    free(me);

    return 0;
}