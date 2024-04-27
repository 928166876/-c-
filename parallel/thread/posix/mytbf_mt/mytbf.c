#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "mytbf.h"


struct mytbf_st{
    int cps;
    int burst;
    int token;
    int pos;
    pthread_mutex_t mut;
    pthread_cond_t con;
};

static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;
static struct mytbf_st *job[MYTBF_MAX];
static pthread_t tid;
static pthread_once_t pth_once = PTHREAD_ONCE_INIT;


static int min(int a, int b){
    if(a < b)
        return a;
    return b;
}


static int get_free_pos_unlock(void){
    for(int i = 0; i < MYTBF_MAX; i++){
        if(job[i] == NULL)
            return i;
    }

    return -1;
}

static void *thr_hand(void * p){
    //alarm(1);
    while(1){
        pthread_mutex_lock(&mut_job);
        for(int i = 0; i < MYTBF_MAX; i++){
            if(job[i] != NULL){
                pthread_mutex_lock(&job[i]->mut);
                job[i]->token += job[i]->cps;
                if(job[i]->token >= job[i]->burst)
                    job[i]->token = job[i]->burst;
                pthread_cond_broadcast(&job[i]->con);
                pthread_mutex_unlock(&job[i]->mut);
            }
        }
        pthread_mutex_unlock(&mut_job);
        sleep(1);
    }
    pthread_exit(NULL);
}

static void mod_unload(void){
    //signal(SIGALRM, alrm_handler_save);
    //alarm(0);
    pthread_cancel(tid);
    pthread_join(tid, NULL);

    for(int i = 0; i < MYTBF_MAX; i++){
        if(job[i] != NULL){
            mytbf_destory(&job[i]);
        }
        free(job[i]);
    }
    pthread_mutex_destroy(&mut_job);
}


static void mod_load(void){
    //alrm_handler_save = signal(SIGALRM, alrm_handler);
    //alarm(1);
    int err;
    err = pthread_create(&tid, NULL, thr_hand, NULL);
    if(err){
        fprintf(stderr, "pthread_creat():%s", strerror(err));
        exit(1);
    }

    atexit(mod_unload);
}


mytbf_t* mytbf_init(int cps, int burst){
    struct mytbf_st *me;

    // if(!inited){
    //     mod_load();
    // }
    pthread_once(&pth_once, mod_load);

    me = malloc(sizeof(*me));
    if(me == NULL){
        return NULL;
    }
    me->token = 0;
    me->cps = cps;
    me->burst = burst;
    pthread_mutex_init(&me->mut, NULL);
    pthread_cond_init(&me->con, NULL);

    pthread_mutex_lock(&mut_job);
    int pos = get_free_pos_unlock();
    if(pos == -1){
        free(me);
        pthread_mutex_unlock(&mut_job);
        return NULL;
        // inited = 1;
    }

    me->pos = pos;
    job[pos] = me;

    pthread_mutex_unlock(&mut_job);
    return me;

}

int mytbf_fetchtoken(mytbf_t *ptr , int size){
    struct mytbf_st *me = ptr;
    if(size < 0){
        return -EINVAL;
    }

    pthread_mutex_lock(&me->mut);
    while(me->token <= 0){
        pthread_cond_wait(&me->con, &me->mut);
        // pthread_mutex_unlock(&me->mut);
        // sched_yield();
        // pthread_mutex_lock(&me->mut);
    }

    int n = min(size, me->token);
    me->token -= n;
    pthread_mutex_unlock(&me->mut);

    return n;
}

int mytbf_returntoken(mytbf_t *ptr , int size){
    struct mytbf_st *me;

    if(size < 0){
        return -EINVAL;
    }
    pthread_mutex_lock(&me->mut);
    me->token += size;
    if(me->token >= me->burst){
        me->token = me->burst;
    }
    pthread_cond_broadcast(&me->con);
    pthread_mutex_unlock(&me->mut);
    return size;
}

int mytbf_destory(mytbf_t* ptr){
    struct mytbf_st *me = ptr;
    pthread_mutex_lock(&mut_job);
    job[me->pos] = NULL;
    pthread_mutex_unlock(&mut_job);

    pthread_mutex_destroy((&me->mut));
    free(me);
    return 0;
}