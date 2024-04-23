#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>


static volatile int loop = 1;

static void alarm_handler(int sig){
    loop = 0;
}

int main(){
    int64_t count = 0;
    signal(SIGALRM, alarm_handler);
    alarm(5);


    while(loop){
        count++;
    }

    printf("%ld\n", count);
    exit(0);
}