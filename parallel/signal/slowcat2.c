#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>


#define CPS         10
#define BUFFERSIZE  CPS
#define BURST       100

static volatile sig_atomic_t token = 0;

static void alrm_handler(int sig){
    alarm(1);
    token++;
    if(token > BURST)
        token = BURST;
}


int main(int argc, char **argv){

    int sfd, dfd = 1;
    int len, ret, pos;
    char buf[BUFFERSIZE];


    if(argc < 2){
        fprintf(stderr, "Usage....\n");
        exit(1);
    }

    signal(SIGALRM, alrm_handler);
    alarm(1);

    do{
        sfd = open(argv[1], O_RDONLY);
        if(sfd < 0){
            if(errno != EINTR){
                perror("open()");
                exit(1);
            }
        }
    }while(sfd < 0);


    while(1){

        while(token <= 0)
            pause();
        token--;
        while((len = read(sfd, buf, BUFFERSIZE)) < 0){
            if(len < 0){
                if(errno == EINTR)
                    continue;
                perror("read()");
                break;
            }
        }
        if(len == 0)
            break;

        pos = 0;
        while(len > 0){
            ret = write(dfd, buf + pos, len);
            if(ret < 0){
                perror("write()");
                exit(1);
            }
            pos += ret;
            len -= ret;
        }
        //sleep(1);
    }

    close(dfd);
    close(sfd);
    
    exit(0);
}