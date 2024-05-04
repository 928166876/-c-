#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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
        dup2(pd[0], 0);
        close(pd[0]);

        int fd = open("/dev/null", O_RDWR);
        dup2(fd, 1);
        dup2(fd, 2);

        execl("/usr/bin/mpg123", "mpg123", "-", NULL);
        perror("execl()");
        exit(0);
    }else{
        close(pd[0]);
        int fd = open("./Aria.mp3", O_RDONLY);
        while(1){
            int len = read(fd, buf, BUFSIZE);
            if(len < 0){
                if(errno == EINTR)
                    continue;
                close(pd[1]);
                exit(1);
            }
            if(len == 0){
                break;
            }
            write(pd[1], buf, len);
        }
        close(pd[1]);
        wait(NULL);
    }


    exit(0);
}