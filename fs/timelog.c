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


    fp = fopen(TFILE, "a+");
    if(fp == NULL){
        perror("fopen()");
        exit(1);
    }

    while(fgets(buf, BUFSIZE,fp) != NULL){
        count++;
    }

    while(1){
        time(&stamp);
        tm = localtime(&stamp);
        strftime(buf, BUFSIZE, "%Y-%m-%d %H:%M:%S",tm);
        fprintf(fp,"%d %s\n", ++count, buf);
        fflush(fp);
        sleep(1);
    }


    fclose(fp);
    exit(1);
}