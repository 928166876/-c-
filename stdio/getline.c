#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv){
    FILE *fp;

    if(argc < 2){
        fprintf(stderr, "Usage:\n");
        exit(1);
    }

    fp = fopen(argv[1], "r");
    if(fp == NULL){
        perror("fopen()");
        exit(1);
    }

    char *linebuf = NULL;
    //char *linebuf;
    size_t linesize = 0;
    //size_t linesize;
    while(1){
        if(getline(&linebuf, &linesize, fp) < 0){
            break;
        }
        printf("%ld\n", strlen(linebuf));
        printf("%ld\n", linesize);
    }
    exit(0);
}