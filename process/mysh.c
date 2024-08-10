#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define DELIMS " \t\n"  //定义可能用到的分隔符

struct cmd_st       //封装一下结构体，保存命令行内容
{   
    glob_t globres;
};


static void prompt(void)
{
    printf("mysh-0.1$ ");   //打印输出提示
}

static void parse(char *line, struct cmd_st *res){  

    char *tok;
    int i = 0;
    while(1)
    {
        tok =  strsep(&line, DELIMS);   //将字符串按指定分隔符 分隔
        if(tok == NULL) //tok == NULL 解析完 
            break;
        if(tok[0] == '\0')    //解析出来空串
            continue;
        // printf("%s\n", tok);
        glob(tok, GLOB_NOCHECK|GLOB_APPEND*i, NULL, &res->globres); //第一次把命令存入res结构体，第二次把飞选项 ，传参，非选项传参保存
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
        prompt();   //每次都先打印提示符

        if(getline(&linebuf, &linebuf_size, stdin) < 0) //从标准输入捕获一行内容 例如： “ls /home/server”
            break;

        parse(linebuf, &cmd);   // 将捕获的内容，和要回填的结构体传递给解析函数

        if(0)
        {
            /*do something*/
        }
        else
        {
            pid = fork();   //创建子进程
            if(pid < 0)     // 判断出错
            {
                perror("fork()");   
                exit(1);
            }

            if(pid == 0)
            {
                fflush(NULL);
                execvp(cmd.globres.gl_pathv[0], cmd.globres.gl_pathv);  //调用exec一组函数，执行命令
                perror("execvp()");
                exit(1);
            }else
            {
                cmd.globres.gl_pathc = 0;   //清零
                cmd.globres.gl_pathv = NULL;    //清空
                wait(NULL); //收尸
            }
        }
        
    }
    globfree(&cmd.globres);     //释放资源，避免内存泄漏

    exit(0);
}