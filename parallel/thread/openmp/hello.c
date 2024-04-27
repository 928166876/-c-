#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void){
#pragma omp parallel
{
    puts("hello");
    puts("world");
}
    exit(0);
}