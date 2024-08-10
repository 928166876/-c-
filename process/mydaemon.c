#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>


#define FNAME "/tmp/out"

static int daemonize()
{
    int fd;
    pid_t pid;
    pid =  fork(); 
    if(pid < 0)
    {
        return -1;  //注意这里没有用perror报错
    }

    if(pid > 0)
    {
        exit(0);    //父进程在这里就退出了！！！
    }
    /*!!! 注意下面都是以子进程的方式在运行！！！*/
    fd = open("/dev/null", O_RDWR);
    if(fd < 0)
    {
        return -1;
    }
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);    // 关闭不用的输入输出
    if(fd > 2)
    {
        close(fd);  //关闭刚才的文件描述符
    }

    setsid();   //现在子进程被1号init进程接管，setsid，将该进程设置为进程组的leader和回话的leader，

    chdir("/"); //修改守护进程的运行路径
     
    return 0;

}


int main(void)
{
    FILE *fp;

    openlog("mydaemon", LOG_PID, LOG_DAEMON);   // 打开日志 守护进程在后台运行，要关闭标准输入、输出、出错，使用日志服务

    if(daemonize())     // 进入守护进程 返回0代表成功， 非零表示失败
    {
        syslog(LOG_ERR, "daemonize faild!");    
    }else
    {
        syslog(LOG_INFO,"daemnize successded!");
    }
    /*下面内容也是子进程的运行代码*/
    fp = fopen(FNAME, "w");     // 打开指定文件
    if(fp == NULL)
    {
        syslog(LOG_ERR, "fopen() faild!");
        exit(1);
    }

    syslog(LOG_INFO, "%s opened!", FNAME);  // 所有syslog都为往打开的日志内输出内容
    for(int i = 0; ; i++)
    {
        fprintf(fp, "%d\n", i);
        fflush(NULL);
        syslog(LOG_DEBUG, "%d write success.", i);  
        sleep(1);
    }

    closelog();     //关闭系统日志，关闭打开的文件描述符
    fclose(fp);
    exit(0);
}