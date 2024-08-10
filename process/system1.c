#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid;
  
    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if(pid == 0)
    {
        execl("/bin/sh", "sh", "-c", "date +%s", NULL); //区别于few.c 这里是调用shell 然后来执行date +%s
        perror("execl()");
        exit(1);
    }

    wait(NULL);     //wait函数参数是状态信息，返回值是pid_t类型
    

    exit(0);
}