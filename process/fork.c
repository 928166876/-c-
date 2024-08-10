#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


int main(void){
    pid_t pid;
    printf("[%d]:begin!\n", getpid());
    fflush(NULL); //flush!!! 没有这句将结果输出到文件当中会打印两个begin
    pid = fork();
    if(pid < 0){
        perror("fork()");
        exit(1);
    }else if(pid == 0){
        //sleep(1);     //永远不要猜测那个任务会被调度器先执行
        printf("[%d]:child is working.\n", getpid());
        printf("[%d]:child end!\n", getpid());
        exit(0);
    }else{
        printf("[%d]:parent is working.\n", getpid());
    }
    // getchar();
    printf("[%d]:parent end!\n", getpid());
    exit(0);
}