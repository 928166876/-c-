#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <error.h>
#include <unistd.h>

#define BUFSIZE 1024
#define THRNUM  20
#define FNAME   "/tmp/out"

static void handler(){
    FILE *fp;
    char buf[BUFSIZE];
    fp = fopen(FNAME, "r+");
    if(fp == NULL){
        perror("fopen()");
        exit(1);
    }
    int fd = fileno(fp);
    lockf(fd, F_LOCK, 0);
    fgets(buf, BUFSIZE, fp);
    fseek(fp, 0, SEEK_SET);
    sleep(1);
    fprintf(fp,"%d\n", atoi(buf)+1);
    fflush(fp);
    lockf(fd, F_ULOCK, 0);
    fclose(fp);

}

int main(void){
    
    pid_t pid;
    for(int i = 0; i < THRNUM; i++){
        pid = fork();
        if(pid < 0){
            perror("fork()");
            exit(1);
        }
        if(pid == 0){
            handler();
            exit(0);
        }

    }

    for(int i = 0; i < THRNUM; i++){
        wait(NULL);
    }   

    exit(0);
}