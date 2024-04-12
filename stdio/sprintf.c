#include <stdio.h>
#include <stdlib.h>
/*****************
 *sprintf atoi fprintf
 *sprintf 可以看作是 atoi的反向操作
 ***************/
#define BUFFERSIZE 1024

int main(void){
    char buf[BUFFERSIZE];

    int year = 2024;
    int month = 04;
    int day = 12;

    sprintf(buf, "%d%d%d", year, month, day);
    printf("%s\n", buf);
    exit(0);
}