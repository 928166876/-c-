#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include "mytbf.h"

// typedef void (*sighandler_t)(int);
struct mytbf_st{
    int cps;
    int burst;
    int token;
    int pos;
};

static struct mytbf_st *job[MYTBF_MAX];
static int inited = 0;
//static sighandler_t alrm_handler_save;
static struct sigaction alrm_sa_save;


static int min(int a, int b){
    if(a < b)
        return a;
    return b;
}


static int get_free_pos(void){
    for(int i = 0; i < MYTBF_MAX; i++){
        if(job[i] == NULL)
            return i;
    }

    return -1;
}

static void alrm_handler(int s, siginfo_t *infop, void *unused){
    // alarm(1);
    if(infop->si_code != SI_KERNEL){
        return;
    }
    for(int i = 0; i < MYTBF_MAX; i++){
        if(job[i] != NULL){
            job[i]->token += job[i]->cps;
            if(job[i]->token >= job[i]->burst)
                job[i]->token = job[i]->burst;
        }
    }
}

static void mod_unload(void){
    // signal(SIGALRM, alrm_handler_save);
    // alarm(0);
    struct itimerval itv;

    sigaction(SIGALRM, &alrm_sa_save, NULL);

    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, NULL);

    for(int i = 0; i < MYTBF_MAX; i++){
        free(job[i]);
    }
}


static void mod_load(void){
    // alrm_handler_save = signal(SIGALRM, alrm_handler);
    // alarm(1);

    struct sigaction sa;
    sa.sa_sigaction = alrm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGALRM, &sa, &alrm_sa_save);
    /*if error*/
    struct itimerval itv;
    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, NULL);
    /*if error*/
    atexit(mod_unload);
    
}


mytbf_t* mytbf_init(int cps, int burst){
    struct mytbf_st *me;

    if(!inited){
        mod_load();
    }

    int pos = get_free_pos();
    if(pos == -1){
        return NULL;
        inited = 1;
    }

    me = malloc(sizeof(*me));
    me->token = 0;
    me->cps = cps;
    me->burst = burst;
    me->pos = pos;

    job[pos] = me;
    return me;

}

int mytbf_fetchtoken(mytbf_t *ptr , int size){
    struct mytbf_st *me = ptr;
    if(size < 0){
        return -EINVAL;
    }

    while(me->token <= 0){
        pause();
    }

    int n = min(size, me->token);
    me->token -= n;
    return n;
}

int mytbf_returntoken(mytbf_t *ptr , int size){
    struct mytbf_st *me;

    if(size < 0){
        return -EINVAL;
    }

    me->token += size;
    if(me->token >= me->burst){
        me->token = me->burst;
    }

    return size;
}

int mytbf_destory(mytbf_t* ptr){
    struct mytbf_st *me = ptr;
    job[me->pos] = NULL;
    free(me);
    return 0;
}