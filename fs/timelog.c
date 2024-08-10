#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 1024
#define TFILE "/tmp/out"

int main(void){
    int count = 0;
    FILE *fp;
    char buf[BUFSIZE];
    time_t stamp;
    struct tm *tm;


    fp = fopen(TFILE, "a+");    //a+是为了在后面追加内容
    if(fp == NULL){
        perror("fopen()");
        exit(1);
    }

    while(fgets(buf, BUFSIZE,fp) != NULL){
        count++;    //统计之前有多少行
    }

    while(1){
        time(&stamp);   //time回填的方式取时间
        tm = localtime(&stamp);
        strftime(buf, BUFSIZE, "%Y-%m-%d %H:%M:%S",tm);
        fprintf(fp,"%d %s\n", ++count, buf);
        fflush(fp);     //文件是全缓冲，一定要刷新缓冲区
        sleep(1);
    }


    fclose(fp);
    exit(1);
}