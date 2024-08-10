#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static off_t flen(const char *fname){
    struct stat statres;
    if(stat(fname, &statres) < 0){  //给文件名或路径名，它给你回填结构体
        perror("stat()");
        exit(1);
    }
    return statres.st_size;
}


int main(int argc, char **argv){

    if(argc < 2){
        fprintf(stderr, "Usage...\n");
        exit(1);
    }

    long long len = flen(argv[1]);
    printf("%lld\n", len);
    exit(0);
}