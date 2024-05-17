#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>

#include "../include/proto.h"
#include "thr_channel.h"
#include "medialib.h"
#include "server_conf.h"

static int tid_nextpos = 0;

struct thr_channel_entry_st{
    chnid_t chnid;
    pthread_t tid;
};


struct thr_channel_entry_st thr_channel[CHANNR];

static void *thr_channel_snder(void *p){
    int len;
    struct msg_channel_st *sbufp;
    struct mlib_listentry_st *entry = p;

    sbufp = malloc(MSG_CHANNEL_MAX);
    if(sbufp == NULL){
        syslog(LOG_ERR, "malloc():%s", strerror(errno));
        exit(1);
    }

    sbufp->chnid = entry->chnid;

    while(1){
        len = mlib_readchn(entry->chnid, sbufp->data, MAX_DATA);
#if DEBUG
        printf("chnid:%d, len:%d\n", entry->chnid, len);
#endif
        if(len < 0){
            break;
        }
        if(sendto(serversd, sbufp, len+sizeof(chnid_t), 0, (void *)&sndaddr, sizeof(sndaddr)) < 0){
            syslog(LOG_ERR, "thr_channel[%d]:sendto() %s.", entry->chnid, strerror(errno));
            break;
        }else{
            syslog(LOG_INFO, "thr_channel[%d]:sendto() successed.", entry->chnid);
        }
        sched_yield();
    }
    pthread_exit(NULL);
}


int thr_channel_create(struct mlib_listentry_st *ptr){
    int err;
    err = pthread_create(&thr_channel[tid_nextpos].tid, NULL, thr_channel_snder, ptr);
    if(err){
        syslog(LOG_WARNING, "pthread_create():%s", strerror(errno));
        return -err;
    }
    thr_channel[tid_nextpos].chnid = ptr->chnid;
    tid_nextpos++;
    return 0;
}


int thr_channel_destory(struct mlib_listentry_st *ptr){
    for(int i = 0; i < CHANNR; i++){
        if(thr_channel[i].chnid == ptr->chnid){
            if(pthread_cancel(thr_channel[i].tid) < 0){
                syslog(LOG_ERR, "pthread_cancel():thr thread of channel%d", ptr->chnid);
                return -ESRCH;
            }
            pthread_join(thr_channel[i].tid, NULL);
            thr_channel[i].chnid = -1;
            break;
        }
    }
    return 0;
}


int thr_channel_destoryall(void){
    for(int i = 0; i < CHANNR; i++){
        if(thr_channel[i].chnid > 0){
            if(pthread_cancel(thr_channel[i].tid) < 0){
                syslog(LOG_ERR, "pthread_cancel():thr thread of channel%d", thr_channel[i].chnid);
                return -ESRCH;
            }
            pthread_join(thr_channel[i].tid, NULL);
            thr_channel[i].chnid = -1;
        }
    }
    return 0;
}
