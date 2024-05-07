#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "mypipe.h"


struct mypipe_st{
    int head;
    int tail;
    char data[PIPESIZE];
    int datasize;
    int count_reader;
    int count_writer;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

mypipe_t  *mypipe_init(){
    struct mypipe_st *me;
    me = malloc(sizeof(*me));
    if(me == NULL)
        return NULL;
    
    me->head = 0;
    me->tail = 0;
    me->datasize = 0;
    pthread_mutex_init(&me->mutex, NULL);
    pthread_cond_init(&me->cond, NULL);

    return me;
}

int mypipe_register(mypipe_t *ptr, int opmap){
    struct mypipe_st *me = ptr;

    pthread_mutex_lock(&me->mutex);
    if(opmap & PIPE_READER){
        me->count_reader++;
    }
    if(opmap & PIPE_WRITER){
        me->count_writer++;
    }

    while(me->count_reader <= 0 || me->count_writer <= 0){
        pthread_cond_wait(&me->cond, &me->mutex);
    }

    pthread_cond_broadcast(&me->cond);
    pthread_mutex_unlock(&me->mutex);

    return 0;

}

int mypipe_unregister(mypipe_t *ptr, int opmap){
    struct mypipe_st *me = ptr;

    pthread_mutex_lock(&me->mutex);
    if(opmap  & PIPE_READER){
        me->count_reader--;
    }
    if(opmap & PIPE_WRITER){
        me->count_writer--;
    }
    pthread_cond_broadcast(&me->cond);

    pthread_mutex_unlock(&me->mutex);
    return 0;
}


static int mypipe_readbyte_unlocked(struct mypipe_st *me, char *datap){
    if(me->datasize <= 0){
        return -1;
    }

    *datap = me->data[me->head];
    me->head = (me->head++)%PIPESIZE;
    me->datasize--;

    return 0;
}

int mypipe_read(mypipe_t *ptr, void *buf, size_t size){
    struct mypipe_st *me = ptr;

    pthread_mutex_lock(&me->mutex);
    while(me->datasize <= 0 && me->count_writer > 0){
        pthread_cond_wait(&me->cond, &me->mutex);
    }

    if(me->datasize <= 0 && me->count_writer <= 0){
        pthread_mutex_unlock(&me->mutex);
        return 0;
    }

    for(int i = 0; i < size; i++){
        if(mypipe_readbyte_unlocked(me, buf+i) != 0)
            break;
    }
    pthread_cond_broadcast(&me->cond);
    pthread_mutex_unlock(&me->mutex);
    return 0;
}

int mypipe_write(mypipe_t *ptr, const void *buf, size_t size){

}

int mypipe_destory(mypipe_t *ptr){
    struct mypipe_st *me = ptr;

    pthread_mutex_destroy(&me->mutex);
    pthread_cond_destroy(&me->cond);
    free(ptr);

    return 0;
}
