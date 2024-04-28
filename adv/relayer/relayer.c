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

#include "relayer.h"


#define BUFSIZE 1024

enum{
    STATE_R = 1,
    STATE_W,
    STATE_Ex,
    STATE_T
};

struct fsm_st{
    int state;
    int sfd;
    int dfd;
    char buf[BUFSIZE];
    int len;
    int pos;
    char *errstr;
};


struct job_st{
    int job_state;
    int fd1, fd2;
    int fd1_save, fd2_save;
    struct fsm_st fsm12, fsm21;
};

static struct job_st *jobs[JOBMAX];
static pthread_mutex_t job_mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t job_once = PTHREAD_ONCE_INIT;


static void fsm_driver(struct fsm_st *fsm){
    int ret;

    switch(fsm->state){
        case STATE_R:
            fsm->len = read(fsm->sfd, fsm->buf, BUFSIZE);
            if(fsm->len == 0){
                fsm->state = STATE_T;
            }else if(fsm->len < 0){
                if(errno == EAGAIN)
                    fsm->state = STATE_R;
                else{
                    fsm->errstr = "read fail";
                    fsm->state = STATE_Ex;
                }
            }else{
                fsm->pos = 0;
                fsm->state = STATE_W;
            }
            break;
        case STATE_W:
            ret = write(fsm->dfd, fsm->buf+fsm->pos, fsm->len);
            if(ret < 0){
                if(errno == EAGAIN)
                    fsm->state = STATE_W;
                else{
                    fsm->errstr = "write fail";
                    fsm->state = STATE_Ex;
                }
            }else{
                fsm->pos += ret;
                fsm->len -= ret;
                if(fsm->len == 0)
                    fsm->state = STATE_R;
                else    
                    fsm->state = STATE_W;
            }
            break;
        case STATE_Ex:
            fsm->state = STATE_T;
            break;
        case STATE_T:
            /*do sth*/
            break;
        default:
            abort();
            break;
    }  
}

static int get_free_pos_unlock(){
    for(int i = 0; i < JOBMAX; i++){
        if(jobs[i] == NULL){
            return i;
        }
    }
    return -1;
}


static void *handler(void *p){
    while(1){
        pthread_mutex_lock(&job_mut);
        for(int i = 0; i < JOBMAX; i++){
            if(jobs[i] != NULL){
                if(jobs[i]->job_state == STATE_RUNNING){
                    fsm_driver(&jobs[i]->fsm12);
                    fsm_driver(&jobs[i]->fsm21);
                    if(jobs[i]->fsm12.state == STATE_T || jobs[i]->fsm21.state == STATE_T){
                        jobs[i]->job_state = STATE_OVER;
                    }
                }
            }
            pthread_mutex_unlock(&job_mut);
        }
    }
    pthread_exit(NULL);
}


static void module_load(){
    pthread_t tid;
    int err = pthread_create(&tid, NULL, handler, NULL);
    if(err){
        fprintf(stderr, "pthread_create() %s\n", strerror(err));
        exit(1);
    }
}


//return >= 0 get job id
//       == -EINVAL 参数非法
//       == -ENOSPC 任务数组满
//       == -ENOMEM 内存分配出错
int relay_addjob(int fd1, int fd2){
    int pos;
    struct job_st *me;
    pthread_once(&job_once, module_load);

    me = malloc(sizeof(*me));
    if(me == NULL){
        return -ENOMEM;
    }

    me->job_state = STATE_RUNNING;
    me->fd1 = fd1;
    me->fd2 = fd2;

    me->fd1_save = fcntl(me->fd1, F_GETFL);
    fcntl(me->fd1, F_SETFL, me->fd1_save|O_NONBLOCK);
    me->fd2_save = fcntl(me->fd2, F_GETFL);
    fcntl(me->fd2, F_SETFL, me->fd2_save);

    me->fsm12.state = STATE_R;
    me->fsm12.sfd = me->fd1;
    me->fsm12.dfd = me->fd2;

    me->fsm21.state = STATE_R;
    me->fsm21.sfd = me->fd2;
    me->fsm21.dfd = me->fd1;

    pthread_mutex_lock(&job_mut);
    pos = get_free_pos_unlock();
    if(pos < 0){
        pthread_mutex_unlock(&job_mut);
        fcntl(me->fd1, F_SETFL, me->fd1_save);
        fcntl(me->fd2, F_SETFL, me->fd2_save);
        free(me);
        return -ENOSPC;
    }
    
    jobs[pos] = me;
    pthread_mutex_unlock(&job_mut);
    return pos;
}