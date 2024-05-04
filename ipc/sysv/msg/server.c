#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "proto.h"


int main(void){
    key_t key;
    int msgid;
    struct msg_st rbuf;
    key = ftok(KEYPATH, KEYPROJ);
    if(key < 0){
        perror("ftok()");
        exit(1);
    }

    msgid = msgget(key, IPC_CREAT|0600);
    if(msgid < 0){
        perror("msgget()");
        exit(1);
    }

    while(1){
        if(msgrcv(msgid, &rbuf, sizeof(rbuf)-sizeof(long), 0, 0) < 0){
            perror("masgrecv()");
            break;
        }

        fprintf(stdout, "name: %s\n", rbuf.name);
        fprintf(stdout, "chinese: %d\n", rbuf.chinese);
        fprintf(stdout, "math: %d\n", rbuf.math);
        fflush(NULL);
    }

    msgctl(key, IPC_RMID, 0);
    exit(0);
}