#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char** argv){
    int count = 0;      //定义一个变量用来计数
    FILE* fp = NULL;

    if(argc < 2){           //判断参数个数
        fprintf(stderr, "Usage:\n");
        exit(1);
    }

    fp = fopen(argv[1], "r");      //打开文件
    if(fp == NULL){             //判断文件是否打开成功
        perror("fopen()");  
        exit(1);
    }

    while(fgetc(fp) != EOF){       //循环读取文件中的字符（从打开的流中读取）
        count++;
    }

    printf("count = %d\n", count);    //输出字符个数
    fclose(fp);                         //关闭文件

    exit(0);    
}