#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <wait.h>

#define BUFSIZE 1024

int main(){
    int len;
    int pd[2];
    pid_t pid;
    char buf[BUFSIZE];

    if(pipe(pd) < 0){
        perror("pipe()");
        exit(1);
    }

    pid = fork();
    if(pid < 0){
        perror("fork()");
        exit(1);
    }
    if(pid == 0){
        close(pd[1]);
        len = read(pd[0], buf, BUFSIZE);
        write(1, buf, len);
        close(pd[0]);
        exit(0);
    }else{
        close(pd[0]);
        write(pd[1], "hello!\n", 7);
        close(pd[1]);
        wait(NULL);
    }


    exit(0);
}