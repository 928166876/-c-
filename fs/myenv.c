#include <stdio.h>
#include <stdlib.h>

extern char **environ;

static void getEnv(const char *key){
    puts(getenv(key));
}

int main(void){

    for(int i = 0; environ[i] != NULL; i++){
        puts(environ[i]);
    }

    getEnv("PATH");
    getchar();
    exit(0);
}