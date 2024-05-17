#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "server_conf.h"
#include "thr_list.h"
#include "medialib.h"

static pthread_t tid_list;
static int num_list_entry;
static struct mlib_listentry_st *list_entry;

static void *thr_list(void *p){
    int size;
    int ret;
    int totalsize;
    struct msg_list_st *entrylistptr;
    struct msg_listentry_st *entryptr;

    totalsize = sizeof(chnid_t);
#if DEBUG   
    printf("sizof:%ld\n", sizeof(struct msg_listentry_st));
#endif
    for(int i = 0; i < num_list_entry; i++){
        totalsize += sizeof(struct msg_listentry_st) + strlen(list_entry[i].desc);
    }
#if DEBUG   
    printf("toalsize:%d\n", totalsize);
#endif
    entrylistptr = malloc(totalsize);
    if(entrylistptr == NULL){
        syslog(LOG_ERR, "malloc():%s", strerror(errno));
        exit(1);
    }

    entrylistptr->chnid = LISTCHNID;
    entryptr = entrylistptr->entry;

    for(int i = 0; i < num_list_entry; i++){
        size = sizeof(struct msg_listentry_st) + strlen(list_entry[i].desc);
#if DEBUG
        printf("size:%d\n", size);
#endif
        entryptr->chnid = list_entry[i].chnid;
        entryptr->len = htons(size);
#if DEBUG
        printf("entryptr->len:%d\n", entryptr->len);
#endif
        strcpy(entryptr->desc, list_entry[i].desc);
        entryptr = (void *)(((char *)entryptr)+ size);
    }

    while(1){
        ret = sendto(serversd, entrylistptr, totalsize, 0, (void *)&sndaddr, sizeof(sndaddr));
        if(ret < 0){
            syslog(LOG_WARNING, "sendto() filed:%s", strerror(errno));
        }else{
            syslog(LOG_DEBUG, "sendto(serversd, enlistp...) success");
        }
        sleep(1);
    }
}

int thr_list_create(struct mlib_listentry_st *listptr, int num_ent){
    int err;
    list_entry = listptr;
    num_list_entry = num_ent;
    err = pthread_create(&tid_list, NULL, thr_list, NULL);
    if(err){
        syslog(LOG_ERR, "pthread_create():%s", strerror(errno));
        return -1;
    }
    return 0;
}


int thr_list_destory(void){
    pthread_cancel(tid_list);
    pthread_join(tid_list, NULL);
    return 0;

}