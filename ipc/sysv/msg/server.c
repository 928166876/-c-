#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct msg_st{
    long type;
    char name[40];
    int math;
    int chinese;
};

int main(){
    key_t key;
    int msgid;
    struct msg_st rbuf;
    int running = 1;
    key = ftok("/etc/services", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }
    while (running) {
        // 接收消息
        if (msgrcv(msgid, &rbuf, sizeof(rbuf), rbuf.type, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("name: %s\n", rbuf.name);
        printf("chinese: %d\n", rbuf.chinese);
        printf("math: %d\n", rbuf.math);

        // 如果消息内容为"exit"，则停止接收
        if (strcmp(rbuf.name, "exit") == 0)
            running = 0;
    }

    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    exit(0);
}