#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>

#include "mytbf.h"

struct mytbf_st{
    int cps;
    int burst;
    int token;
    int pos;
    pthread_mutex_t mut;
    pthread_cond_t cond;
};

static struct mytbf_st *jobs[MYTBF_MAX];
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;
static pthread_t tid;


static int min(int a, int b){return a < b ? a : b;}

static int get_free_pos_unlock(){
    for(int i = 0; i < MYTBF_MAX; i++){
        if(jobs[i] == NULL)
            return i;
    }
    return -1;
}

static void *thr_alrm(void *p){

    while(1){
        pthread_mutex_lock(&mut_job);
        for(int i = 0; i < MYTBF_MAX; i++){
            if(jobs[i] != NULL){
                pthread_mutex_lock(&jobs[i]->mut);
                jobs[i]->token += jobs[i]->cps;
                if(jobs[i]->token > jobs[i]->burst)
                    jobs[i]->token = jobs[i]->burst;
                pthread_cond_broadcast(&jobs[i]->cond);
                pthread_mutex_unlock(&jobs[i]->mut);
            }
        }
        pthread_mutex_unlock(&mut_job);
        sleep(1);
    }
}


static void module_unload(){
    int i;
    pthread_cancel(tid);
    pthread_join(tid, NULL);

    for(int i = 0; i < MYTBF_MAX; i++){
        free(jobs[i]);
    }
}


static void module_load(){

    int err;
    err = pthread_create(&tid, NULL, thr_alrm, NULL);
    if(err){
        syslog(LOG_ERR, "pthread_create():%s.", strerror(errno));
        exit(1);
    }

    atexit(module_unload);
}


mytbf_t *mytbf_init(int cps, int burst){
    int pos;
    struct mytbf_st *me;

    pthread_once(&init_once, module_load);

    me = malloc(sizeof(*me));
    if(me == NULL)
        return NULL;
    me->cps = cps;
    me->burst = burst;
    me->token = 0;
    pthread_mutex_init(&me->mut, NULL);
    pthread_cond_init(&me->cond, NULL);

    pthread_mutex_lock(&mut_job);

    pos = get_free_pos_unlock();
    if(pos < 0){
        pthread_mutex_unlock(&mut_job);
        free(me);
        exit(1);
    }

    me->pos = pos;
    jobs[me->pos] = me;
    pthread_mutex_unlock(&mut_job);
    return me;

}

int mytbf_fetchtoken(mytbf_t *ptr,int size){
    int n;
    struct mytbf_st *me = ptr;
    pthread_mutex_lock(&me->mut);
    while(me->token <= 0)
        pthread_cond_wait(&me->cond, &me->mut);
    
    n = min(me->token, size);
    me->token -= n;
    pthread_mutex_unlock(&me->mut);
    return n;
}

int mytbf_returntoken(mytbf_t *ptr, int size){
    struct mytbf_st *me = ptr;
    pthread_mutex_lock(&me->mut);
    me->token += size;
    while(me->token > me->burst)
        me->token = me->burst;

    pthread_mutex_unlock(&me->mut);
    return 0;
}

int mytbf_destory(mytbf_t *ptr){
    struct mytbf_st *me = ptr;
    pthread_mutex_lock(&mut_job);
    jobs[me->pos] = NULL;
    pthread_mutex_unlock(&mut_job);

    pthread_mutex_destroy(&me->mut);
    pthread_cond_destroy(&me->cond);
    free(ptr);
    return 0;
}