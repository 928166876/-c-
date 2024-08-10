#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FNAME "/tmp/out"

int main(void){
    
    close(1);           //puts先关闭标准输出，优先使用文件描述符小的位置
    int fd = open(FNAME, O_WRONLY|O_CREAT|O_TRUNC, 0600); //打开文件 0600是权限
    if(fd < 0){
        perror("open()");
        exit(1);
    }



    /*==============================*/
    puts("hello,world!");   //带自动换行
    exit(0);
}