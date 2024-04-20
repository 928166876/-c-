#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define DELIMS " \t\n"

struct cmd_st
{
    glob_t globres;
};


static void prompt(void)
{
    printf("mysh-0.1$ ");
}

static void parse(char *line, struct cmd_st *res){

    char *tok;
    int i = 0;
    while(1)
    {
        tok =  strsep(&line, DELIMS);
        if(tok == NULL)
            break;
        if(tok[0] == '\0')
            continue;
        
        glob(tok, GLOB_NOCHECK|GLOB_APPEND*i, NULL, &res->globres);
        i = 1;
    }
}

int main(void)
{
    pid_t pid;
    char *linebuf;
    size_t linebuf_size = 0;
    struct cmd_st cmd;

    while(1){
        prompt();

        if(getline(&linebuf, &linebuf_size, stdin) < 0)
            break;

        parse(linebuf, &cmd);

        if(0)
        {
            /*do something*/
        }
        else
        {
            pid = fork();
            if(pid < 0)
            {
                perror("fork()");
                exit(1);
            }

            if(pid == 0)
            {
                fflush(NULL);
                execvp(cmd.globres.gl_pathv[0], cmd.globres.gl_pathv);
                perror("execvp()");
                exit(1);
            }else
            {
                cmd.globres.gl_pathc = 0;
                cmd.globres.gl_pathv = NULL;
                wait(NULL);
            }
        }
        
    }
    globfree(&cmd.globres);

    exit(0);
}