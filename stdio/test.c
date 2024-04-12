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

    while(fgets(buf, BUFFERSIZE, fp)){
        printf("%s\n", buf);
    }
    
    exit(0);
}