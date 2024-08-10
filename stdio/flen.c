#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char** argv){
    int count = 0;
    FILE* fp = NULL;

    if(argc < 2){
        fprintf(stderr, "Usage:\n");
        exit(1);
    }

    fp = fopen(argv[1], "r");
    if(fp == NULL){
        perror("fopen()");
        exit(1);
    }
    fseek(fp, 0, SEEK_END);     //将文件指针移动到文件末尾
    printf("%ld\n", ftell(fp)); //输出文件指针的位置
/*
    while(fgetc(fp) != EOF){
        count++;
    }

    printf("count = %d\n", count);
*/

    fclose(fp);

    exit(0);    
}