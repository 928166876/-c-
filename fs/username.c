#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

int main(int argc, char **argv){
    struct passwd *pwdline;

    if(argc < 2){
        fprintf(stderr, "Usage....\n");
        exit(1);
    }

    pwdline = getpwuid(atoi(argv[1]));  //根据用户编号拿到用户信息结构体
    puts(pwdline->pw_name);     //从结构体取数据
    // puts(pwdline->pw_passwd); //权限限制，拿不到passwd

    exit(0);
}