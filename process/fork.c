#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


int main(void){
    pid_t pid;
    printf("[%d]:begin!\n", getpid());
    fflush(NULL); //flush!!!
    pid = fork();
    if(pid < 0){
        perror("fork()");
        exit(1);
    }else if(pid == 0){
        //sleep(1);
        printf("[%d]:child is working.\n", getpid());
    }else{
        printf("[%d]:parent is working.\n", getpid());
    }
    //getchar();
    printf("[%d]:end!\n", getpid());
    exit(0);
}