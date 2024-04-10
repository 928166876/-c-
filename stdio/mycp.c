#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv){

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

    int res = 0;
    while (1){
        res = fgetc(fps);
        if(res == EOF){
            break;
        }
        fputc(res, fpd);
    }

    fclose(fps);
    fclose(fpd);

    exit(0);
}