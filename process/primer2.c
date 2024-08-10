#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


#define LEFT 30000000
#define RIGHT 30000200

int main(void){
    int mark;
    pid_t pid;
    for(int i = LEFT; i <= RIGHT; i++){
        fflush(NULL);
        pid = fork();
        if(pid < 0){
            perror("fork()");
            exit(1);
        }
        if(pid == 0){   //创建RIGHT - LEFT个进程负责筛质数
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
            exit(0);
        }
    }

    // pid_t st;
    for(int i = LEFT; i <= RIGHT; i++){ //父进程创建后等待收尸
        wait(NULL);
        // st = wait(NULL);        //用上wait的返回值，接收返回子进程的pid
        // printf("%d end.\n", st);    
    }

    exit(0);
}