#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define N 3
#define LEFT 30000000
#define RIGHT 30000200

int main(void){
    int mark;
    pid_t pid;
    for(int n = 0; n < N; n++){     // 最多创建N个进程
        pid = fork();
        if(pid < 0){
            perror("fork()");
            exit(1);
        }

        if(pid == 0){
            for(int i = LEFT + n; i <= RIGHT; i += N){  //交叉分配的方式，做计算， 任务分配不均匀
                mark = 1;
                for(int j = 2; j < i/2; j++){
                    if(i%j == 0){
                        mark = 0;
                        break;
                    }
                }      
                if(mark){
                    printf("[%d]%d is a primer.\n", n, i);
                }
            }           //每个进程计算了多个数，然后退出
            exit(0);    
        }
    }



    //int st;
    for(int n = 0; n < N; n++){ 
        wait(NULL);
        //printf("%d end.\n", st);
    }

    exit(0);
}