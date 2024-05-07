#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

// 消息结构体
struct message {
    long mtype;
    char mtext[40];
    int math;
    int chinese;
};

int main() {
    key_t key; // 用于获取消息队列键
    int msgid; // 消息队列标识符
    struct message msg; // 消息结构体
    int running = 1; // 接收消息的标志

    // 创建唯一的键
    key = ftok("/etc/services", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    // 获取消息队列
    //msgid = msgget(key, 0666);
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    while (running) {
        // 接收消息
        if (msgrcv(msgid, &msg, sizeof(msg), msg.mtype, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        // 打印消r
        printf("Message received: %s\n", msg.mtext);
        printf("math: %d\n", msg.math);
        printf("chinese: %d\n", msg.chinese);

        // 如果消息内容为"exit"，则停止接收
        if (strcmp(msg.mtext, "exit") == 0)
            running = 0;
    }

    // 删除消息队列
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    printf("Message queue deleted\n");

    return 0;
}