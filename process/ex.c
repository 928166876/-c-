#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    puts("begin.");
    fflush(NULL);       
    execl("/bin/date", "date", "+%s", NULL);    //exec函数组，替换当前进程执行内容
    perror("execl()");
    exit(1);

    puts("end.");

    exit(0);
}