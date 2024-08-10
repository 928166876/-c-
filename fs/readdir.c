#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#define PAT "/etc"

int main(void){
    DIR *dp;    //创建一个目录流指针
    struct dirent *cur;

    dp = opendir(PAT);  //像操作文件一样，打开一个目录文件
    if(dp == NULL){
        perror("opendir()");
        exit(1);
    }

    while((cur = readdir(dp)) != NULL){ //读取目录文件中的目录项 到结构体指针
        puts(cur->d_name);
    };

    closedir(dp);   //关闭目录项



    exit(0);
}