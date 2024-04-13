#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FNAME "/tmp/out"

int main(void){
    
    
    int fd = open(FNAME, O_WRONLY|O_CREAT|O_TRUNC, 0600);
    if(fd < 0){
        perror("open()");
        exit(1);
    }
    dup2(fd, 1);
    if(fd != 1){
        close(fd);
    }
    
    /*==============================*/
    puts("hello,world!");
    exit(0);
}