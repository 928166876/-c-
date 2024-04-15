#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static int ftype(const char *fname){
    struct stat statres;
    if(stat(fname, &statres) < 0){
        perror("stat()");
        exit(1);
    }
    if(S_ISREG(statres.st_mode))
        return '-';
    else if(S_ISDIR(statres.st_mode))
            return 'd';
        else if(S_ISCHR(statres.st_mode))
                return 'c';
            else    
                return '?';
}

int main(int argc, char **argv){

    int res = ftype(argv[1]);
    printf("%c\n", res);
    exit(0);
}