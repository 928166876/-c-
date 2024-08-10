#include <stdio.h>
#include <stdlib.h>

#define BUFFERSIZE 5

int main(){
    char buf[BUFFERSIZE];   
    FILE *fp;
    fp = fopen("./tmp", "r");
    if(fp == NULL){
        perror("fopen()");
        exit(1);
    }

    while(fgets(buf, BUFFERSIZE, fp)){ //BUFFERSIZE为5最多可以读取4个字符
        printf("%s\n", buf);
    }
    
    exit(0);
}