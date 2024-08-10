#include <stdio.h>
#include <stdlib.h>

#define BUFFERSIZE 1024

int main(int argc, char** argv){
    char buf[BUFFERSIZE];   //定义一个缓冲区

    if(argc < 3){           //判断参数个数
        fprintf(stderr, "Usage:%s <src_file_name> <dest_file_name>\n", argv[0]);
        exit(1);
    }
    FILE *fps;
    FILE *fpd;      //定义两个文件指针

    fps = fopen(argv[1], "r");  //打开源文件
    if(fps == NULL){
        perror("fopen()");
        exit(1);
    }
    fpd = fopen(argv[2], "w");  //打开目标文件
    if(fpd == NULL){
        fclose(fps);
        perror("fopen()");
        exit(1);
    }

    int res = 0;
    while (fgets(buf, BUFFERSIZE, fps)){    //读取一行
        res++;
        fputs(buf, fpd);    //写入目标文件
    }
    fprintf(stdout, "res = %d\n", res);
    fclose(fps);
    fclose(fpd);
    exit(0);
}