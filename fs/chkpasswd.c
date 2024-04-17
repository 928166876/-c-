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

    input_passwd = getpass("PassWord:");
    if(input_passwd == NULL){
        perror("getpass()");
        exit(1);
    }
    shadowline = getspnam(argv[1]);
    crypted_passwd = crypt(input_passwd, shadowline->sp_pwdp);

    if(strcmp(shadowline->sp_pwdp, crypted_passwd) == 0){
        puts("ok!");
    }else{
        puts("faild!\n");
    }

    exit(0);
}