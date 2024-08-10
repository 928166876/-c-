#include <stdio.h>
#include <stdlib.h>

extern char **environ;  //一级指针数组，每个元素都是一条环境变量

static void getEnv(const char *key){
    puts(getenv(key));  //根据name可以取出value
}

int main(void){

    for(int i = 0; environ[i] != NULL; i++){
        puts(environ[i]);  //输出每条环境变量信息
    }

    puts("");
    getEnv("PATH"); 
    getchar();
    exit(0);
}