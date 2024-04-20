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
    for(int n = 0; n < N; n++){
        pid = fork();
        if(pid < 0){
            perror("fork()");
            exit(1);
        }

        if(pid == 0){
            for(int i = LEFT + n; i <= RIGHT; i += N){
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
            }
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