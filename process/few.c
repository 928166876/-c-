#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


// fork + exec* + wait构建了Linux整个世界
int main(void)
{
    pid_t pid;
    puts("begin");

    fflush(NULL);
    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if(pid == 0)
    {
        // sleep(1000);    //测试语句
        execl("/bin/date", "date", "+%s", NULL);
        perror("execl()");
        exit(1);
    }

    wait(NULL);     //父进程等待收尸
    puts("end");

    exit(0);
}