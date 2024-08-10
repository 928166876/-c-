#include <stdio.h>
#include <stdlib.h>
/*****************
 *sprintf atoi fprintf
 *sprintf 可以看作是 atoi的反向操作
 ***************/
#define BUFFERSIZE 1024

int main(void){
    char buf[BUFFERSIZE]; //定义一个缓冲区

    int year = 2024;
    int month = 04;
    int day = 12;

    sprintf(buf, "%d%d%d", year, month, day);  //将数据写入缓冲区 2024412有问题啊不应该是20240412吗
    printf("%s\n", buf);    //打印缓冲区
    exit(0);
}