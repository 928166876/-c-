#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv){
    FILE *fp;

    if(argc < 2){       //判断参数个数
        fprintf(stderr, "Usage:\n");
        exit(1);
    }

    fp = fopen(argv[1], "r");  //打开文件
    if(fp == NULL){
        perror("fopen()");
        exit(1);
    }

    char *linebuf = NULL;   //定义一个指针用来存储读取的行
    //char *linebuf;
    size_t linesize = 0;   //定义一个变量用来存储读取的行的长度
    //size_t linesize;
    while(1){
        if(getline(&linebuf, &linesize, fp) < 0){   //读取一行
            break;
        }
        printf("%ld\n", strlen(linebuf));  //输出读取的每行字符串的长度 19/20.....
        printf("%ld\n", linesize);      //输出 120大小开始增长
    }
    exit(0);
}


/*
10
120
2
120
1
120
1
120
684
685

*/