#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    puts("begin.");
    fflush(NULL);
    execl("/bin/date", "date", "+%s", NULL);
    perror("execl()");
    exit(1);

    puts("end.");

    exit(0);
}