#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define NAMESIZE 40
struct msg_st{
    long type;
    char name[NAMESIZE];
    int math;
    int chinese;
};

int main(void){
    key_t key;
    int msgid;
    struct msg_st sbuf;
    key = ftok("/etc/services", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    msgid = msgget(key, 0666);
    //msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    // 准备消息内容
    strcpy(sbuf.name, "zero");
    sbuf.type = 1; // 可以设置不同的消息类型
    sbuf.math = 87;
    sbuf.chinese = 83;
    // 发送消息
    if (msgsnd(msgid, &sbuf, sizeof(sbuf), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    fprintf(stdout, "OK\n");

    msgctl(msgid, IPC_RMID, NULL);


    exit(0);
}