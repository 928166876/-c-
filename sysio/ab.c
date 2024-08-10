#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){

    putchar('a');   
    write(1,"b",1);     //参数描述 （标准输出， 输出字符串， 长度）

    
    putchar('a');
    write(1,"b",1);
    
    putchar('a');
    write(1,"b",1);
    exit(0);
}

//打印bbbaaa，因为putchar是标准IO，write是系统IO，标准IO是行缓冲，系统IO是无缓冲模式。
