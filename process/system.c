#include <stdio.h>
#include <stdlib.h>


int main(void)
{
    
    system("date +%s > /tmp/out"); // 相当于fork + exec + wait的一种封装，  execl("/bin/sh", "sh", "-c", command, (char *) 0);
    
    exit(0);
}