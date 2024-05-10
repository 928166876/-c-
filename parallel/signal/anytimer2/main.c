#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "anytimer.h"

static void f3(void *p){
    printf("f3():%s\n", (char *)p);
}


static void f2(void *p){
    printf("f2():%s\n", (char *)p);
}

static void f1(void *p){
    printf("f1():%s\n", (char *)p);
}

int main(void){
    int job1;

    puts("Begin!");
    job1 = at_addjob_repeat(5, f1, "aaa");
    if(job1 < 0){
        fprintf(stderr, "at_addjob():%s\n", strerror(-job1));
        exit(1);
    }
#if 0
    at_addjob(2, f2, "bbb");
    at_addjob(7, f3, "ccc");
    puts("End!");
#endif
    while(1){
        write(1, ".", 1);
        sleep(1);
    }
    exit(0);
}