#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#define LEFT 30000000
#define RIGHT 30000200

int main(void){
    int mark;
    pid_t pid;
    for(int i = LEFT; i <= RIGHT; i++){
        fflush(NULL);
        pid = fork();       //创建进程
        if(pid < 0){
            perror("fork()");   //出错，使用perror报错
            exit(1);
        }
        if(pid == 0){   //子进程执行代码
            mark = 1;
            for(int j = 2; j < i/2; j++){
                if(i%j == 0){
                    mark = 0;
                    break;
                }
            }      
            if(mark){
                printf("%d is a primer.\n", i);
            }
            sleep(1000);    //测试父进不收尸，子进程成为孤儿进程
            exit(0);    //！！！注意，一定要执行完退出，避免递归创建进程
        }
    }
    // sleep(1000);    // 1、 此时子进程都执行完，但是没有人给他们收尸，所以他们成为僵尸进程，父进程退出后，子进程会被操作系统回收
    // for(int i = LEFT; i <= RIGHT; i++){
    //     wait(NULL);      // 2、 这里加入收尸操作？
    // }
    exit(0);    
}