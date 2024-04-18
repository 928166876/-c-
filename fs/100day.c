#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIMESTRSIZE 1024

int main(void){
    time_t stamp;
    struct tm *tm;
    char timestr[TIMESTRSIZE];

    stamp = time(NULL);
    tm = localtime(&stamp);
    strftime(timestr, TIMESTRSIZE, "now: %Y-%m-%d", tm);
    puts(timestr);

    tm->tm_mday += 100;
    (void)mktime(tm);
    strftime(timestr, TIMESTRSIZE, "now: %Y-%m-%d", tm);
    puts(timestr);
    exit(0);
}