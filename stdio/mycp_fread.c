#include <stdio.h>
#include <stdlib.h>

#define BUFFERSIZE 1024

int main(int argc, char** argv){
    int n = 0;
    char buf[BUFFERSIZE];

    if(argc < 3){
        fprintf(stderr, "Usage:%s <src_file_name> <dest_file_name>\n", argv[0]);
        exit(1);
    }
    FILE *fps;
    FILE *fpd;

    fps = fopen(argv[1], "r");
    if(fps == NULL){
        perror("fopen()");
        exit(1);
    }
    fpd = fopen(argv[2], "w");
    if(fpd == NULL){
        fclose(fps);
        perror("fopen()");
        exit(1);
    }

    //int res = 0;
    while (n = fread(buf, 1, BUFFERSIZE, fps)){
        printf("%d\n", n);
        fwrite(buf, 1, n, fpd);
    }

    fclose(fps);
    fclose(fpd);
    exit(0);
}