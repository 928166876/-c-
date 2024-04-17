#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <glob.h>

#define PATHSIZE 1024

static int path_noloop(const char *path){
    char *pos;
    pos = strrchr(path, '/');
    if(pos == NULL){
        exit(1);
    }

    if(strcmp(pos+1, ".") == 0 || strcmp(pos+1, "..") == 0){
        return 0;
    }
    return 1;
}


static int64_t mydu(const char *path){
    int sum = 0;
    static struct stat statres;
    glob_t globres;
    static char nextpath[PATHSIZE];

    if(lstat(path, &statres) < 0){
        perror("lstat()");
        exit(1);
    }

    if(!S_ISDIR(statres.st_mode)){
        return statres.st_blocks;
    }

    //处理目录
    strncpy(nextpath, path, PATHSIZE);
    strncat(nextpath, "/*", PATHSIZE);
    if(glob(nextpath, 0, NULL, &globres) < 0){
        fprintf(stderr, "glob()");
        exit(1);
    }
    
    strncpy(nextpath, path, PATHSIZE);
    strncat(nextpath, "/.*", PATHSIZE);
    if(glob(nextpath, GLOB_APPEND, NULL, &globres) < 0){
        fprintf(stderr, "glob()");
        exit(1);
    }

    sum += statres.st_blocks;
    for(int i = 0; i < globres.gl_pathc; i++){
        if(path_noloop(globres.gl_pathv[i])){
            sum += mydu(globres.gl_pathv[i]);
        }
    }
    

    globfree(&globres);
    return sum;
}



int main(int argc, char **argv){
    if(argc < 2){
        fprintf(stderr, "Usage...\n");
        exit(1);
    }

    printf("%ld\n",mydu(argv[1])/2);

    exit(0);
}