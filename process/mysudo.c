#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*./mysudo 0 cat /etc/shadow*/

int main(int argc, char **argv)
{
    pid_t pid;
    if(argc < 3)    //  传参数个数判断
    {
        fprintf(stderr, "Usage...\n");
        exit(1);
    }

    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if(pid == 0)
    {
        setuid(atoi(argv[1]));  //setuid将当前设置为0号用户
        execvp(argv[2], argv+2);    //第一个参数是要执行的命令名， 第二参数是命令名+内容
        perror("execvp()");
        exit(1);
    }

    wait(NULL);

    exit(0);
}