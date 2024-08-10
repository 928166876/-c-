#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


#define BUFFERSIZE 1024

//用系统调用io实现cp命令

int main(int argc, char **argv){

    int sfd, dfd;
    int len, ret, pos;
    char buf[BUFFERSIZE];  //定义一个缓冲区


    if(argc < 3){       //判断参数个数
        fprintf(stderr, "Usage....\n");
        exit(1);
    }


    sfd = open(argv[1], O_RDONLY); //打开源文件
    if(sfd < 0){        //判断文件是否打开成功
        perror("open()");
        exit(1);
    }
    dfd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0600); //打开目标文件
    if(dfd < 0){        //判断文件是否打开成功
        close(sfd);     //关闭源文件
        perror("open()");
        exit(1);
    }
    while(1){
        len = read(sfd, buf, BUFFERSIZE);  //读取数据
        if(len < 0){
            perror("read()");
            break;
        }
        if(len == 0)    //读取结束
            break;

        pos = 0;
        while(len > 0){     
            ret = write(dfd, buf + pos, len); //写入数据
            if(ret < 0){
                perror("write()");
                exit(1);
            }
            pos += ret;     //更新偏移量
            len -= ret;     //更新剩余长度,坚持写够len个字节
        }
        
    }

    close(dfd);
    close(sfd);
    
    exit(0);
}