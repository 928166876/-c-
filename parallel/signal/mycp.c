#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


#define BUFFERSIZE 1024

int main(int argc, char **argv){

    int sfd, dfd;
    int len, ret, pos;
    char buf[BUFFERSIZE];


    if(argc < 3){
        fprintf(stderr, "Usage....\n");
        exit(1);
    }

    do{
        sfd = open(argv[1], O_RDONLY);
        if(sfd < 0){
            if(errno != EINTR){
                perror("open()");
                exit(1);
            }
        }
    }while(sfd < 0);


    do{
        dfd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0600);
        if(dfd < 0){
            close(sfd);
            perror("open()");
            exit(1);
        }
    }while(dfd < 0);

    while(1){
        len = read(sfd, buf, BUFFERSIZE);
        if(len < 0){
            if(errno == EINTR)
                continue;
            perror("read()");
            break;
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
        
    }

    close(dfd);
    close(sfd);
    
    exit(0);
}