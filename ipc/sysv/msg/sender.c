#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

// 消息结构体
struct message {
    long mtype; // 消息类型
    char mtext[40]; // 消息文本
    int math;
    int chinese;
};

int main() {
    key_t key; // 用于获取消息队列键
    int msgid; // 消息队列标识符
    struct message msg; // 消息结构体

    // 创建唯一的键
    key = ftok("/etc/services", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    // 获取消息队列
    msgid = msgget(key, 0666);
    //msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    // 准备消息内容
    strcpy(msg.mtext, "zero");
    msg.mtype = 1; // 可以设置不同的消息类型
    msg.math = 87;
    msg.chinese = 83;
    // 发送消息
    if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    printf("Message sent\n");

    // 通常，发送者在这里结束，不会删除消息队列
    // msgctl(msgid, IPC_RMID, NULL);

    return 0;
}