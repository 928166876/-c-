#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <shadow.h>
#include <string.h>
#include <crypt.h>

int main(int argc, char **argv){
    char *input_passwd = NULL;
    char *crypted_passwd;
    struct spwd *shadowline;

    if(argc < 2){
        fprintf(stderr, "Usage...\n");
        exit(1);
    }

    input_passwd = getpass("PassWord:");    //专门用来接收password 关闭回显功能
    if(input_passwd == NULL){
        perror("getpass()");
        exit(1);
    }
    shadowline = getspnam(argv[1]);
    crypted_passwd = crypt(input_passwd, shadowline->sp_pwdp);  // salt is a character string starting with the characters "$id$ followed by a string terminated by "$"

    if(strcmp(shadowline->sp_pwdp, crypted_passwd) == 0){      //比较加密后的口令与原口令是否相同
        puts("ok!");
    }else{
        puts("faild!\n");
    }

    exit(0);
}