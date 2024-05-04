#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#include "proto.h"

int main(void){

    key_t key;
    int msgid;
    struct msg_st sbuf;
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

    strcpy(sbuf.name, "zero");
    sbuf.chinese = rand() % 100;
    sbuf.math = rand() % 100;
    if(msgsnd(msgid, &sbuf, sizeof(sbuf) - sizeof(long), 0)  < 0){
        perror("msgsnd()");
        exit(1);
    }

    fprintf(stdout, "OK\n");

    msgctl(msgid, IPC_RMID, 0);


    exit(0);
}