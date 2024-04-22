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
        return -1;
    }

    if(pid > 0)
    {
        exit(0);
    }

    fd = open("/dev/null", O_RDWR);
    if(fd < 0)
    {
        return -1;
    }
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    if(fd > 2)
    {
        close(fd);
    }

    setsid();

    chdir("/");
     
    return 0;

}


int main(void)
{
    FILE *fp;

    openlog("mydaemon", LOG_PID, LOG_DAEMON);

    if(daemonize())
    {
        syslog(LOG_ERR, "daemonize faild!");   
    }else
    {
        syslog(LOG_INFO,"daemnize successded!");
    }

    fp = fopen(FNAME, "w");
    if(fp == NULL)
    {
        syslog(LOG_ERR, "fopen() faild!");
        exit(1);
    }

    syslog(LOG_INFO, "%s opened!", FNAME);
    for(int i = 0; ; i++)
    {
        fprintf(fp, "%d\n", i);
        fflush(NULL);
        syslog(LOG_DEBUG, "%d write success.", i);
        sleep(1);
    }

    closelog();
    fclose(fp);
    exit(0);
}