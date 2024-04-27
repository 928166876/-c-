#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void){
#pragma omp parallel sections
{
#pragma omp section
    printf("hello %d\n", omp_get_thread_num());
#pragma omp section
    printf("world %d\n", omp_get_thread_num());
}
    exit(0);
}