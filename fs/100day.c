#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIMESTRSIZE 1024

int main(void){
    time_t stamp;
    struct tm *tm;
    char timestr[TIMESTRSIZE];

    stamp = time(NULL);         //从机器获取的大整数
    tm = localtime(&stamp);     //localtime 将大整数转换为struct tm类型的结构体
    strftime(timestr, TIMESTRSIZE, "now: %Y-%m-%d", tm);    //将结构体的内容按格式输出为字符串
    puts(timestr);

    tm->tm_mday += 100;
    (void)mktime(tm);   //将结构体转换回大整数，这里利用副作用，来让系统增加100天
    strftime(timestr, TIMESTRSIZE, "now: %Y-%m-%d", tm);
    puts(timestr);
    exit(0);
}