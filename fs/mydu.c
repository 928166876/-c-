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
    pos = strrchr(path, '/');   //找到字符串最右边第一 目标字符的位置
    if(pos == NULL){
        exit(1);
    }

    if(strcmp(pos+1, ".") == 0 || strcmp(pos+1, "..") == 0){    //判断是不是本级目录或上级目录
        return 0;
    }
    return 1;
}


static int64_t mydu(const char *path){
    int sum = 0;
    static struct stat statres;     // 存放当前目录或文件的信息
    glob_t globres;         //解析目录时使用
    static char nextpath[PATHSIZE]; //存放路径

    if(lstat(path, &statres) < 0){  //lstat主要处理连接文件，我们要的是连接文件本身的信息
        perror("lstat()");
        exit(1);
    }

    if(!S_ISDIR(statres.st_mode)){  //判断是不是目录文件，不是则直接返回其block数
        return statres.st_blocks;
    }

    //处理目录
    strncpy(nextpath, path, PATHSIZE);  //原始路径拷贝到nextpath
    strncat(nextpath, "/*", PATHSIZE);  //在后面追加"/*"
    if(glob(nextpath, 0, NULL, &globres) < 0){  //解析pattern 将结果存在glob_t结构体中
        fprintf(stderr, "glob()");
        exit(1);
    }
    
    strncpy(nextpath, path, PATHSIZE);
    strncat(nextpath, "/.*", PATHSIZE); //同上解析隐藏文件
    if(glob(nextpath, GLOB_APPEND, NULL, &globres) < 0){    //GLOB_APPEND以追加模式保存
        fprintf(stderr, "glob()");
        exit(1);
    }

    sum += statres.st_blocks;
    for(int i = 0; i < globres.gl_pathc; i++){
        if(path_noloop(globres.gl_pathv[i])){
            sum += mydu(globres.gl_pathv[i]);       //递归调用
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

    printf("%ld\n",mydu(argv[1])/2);    //mydu返回的是block数

    exit(0);
}