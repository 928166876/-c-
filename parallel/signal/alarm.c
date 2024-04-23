#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){

    alarm(1);
    alarm(5);
    alarm(10);

    while(1)
        pause();

    exit(0);
}