#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/if.h>

#include "../include/proto.h"
#include "server_conf.h"
#include "medialib.h"
#include "thr_list.h"
#include "thr_channel.h"
#include "mytbf.h"

int serversd;
struct sockaddr_in sndaddr;
struct mlib_listentry_st *list;
/*
*   -P      specify receive port
*   -M      specify multicast group
*   -F      specify foreground runmode
*   -D      specify medialib location
*   -I      specify net card
*   -H      show help
*/
struct server_conf_st server_conf = {.rcvport = DEFAULT_RCVPORT,
                                    .media_dir = DEFAULT_MEDIADIR,
                                    .runmode = RUN_DAEMON,
                                    .ifname = DEFAULT_IF,
                                    .mgroup = DEFAULT_MGROUP};


static void print_help(){
    printf("-P      specify receive port\n");   
    printf("-M      specify multicast group\n");
    printf("-F      specify foreground runmode\n");
    printf("-D      specify medialib location\n");
    printf("-I      specify net card\n");
    printf("-H      show help\n");
}

static void daemon_exit(int s){
    thr_list_destory();
    thr_channel_destoryall();
    mlib_freechnlist(list);
    syslog(LOG_WARNING, "signal-%d caught, exit now.", s);
    closelog();
    exit(0);
}


static int daemonize(){
    pid_t pid;
    int fd;
    pid = fork();
    if(pid < 0){
        // perror("fork()");
        syslog(LOG_ERR, "fork() failed:%s", strerror(errno));
        return -1;
    }
    if(pid > 0)
        exit(0);


    fd = open("/dev/null", O_RDWR);
    if(fd < 0){
        // perror("open()");
        syslog(LOG_ERR, "open() failed:%s", strerror(errno));
        return -2;
    }

    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    if(fd > 2)
        close(fd);

    setsid();
    chdir("/");
    umask(0);
    return 0;
}

static void socket_init(){
    struct ip_mreqn mreq;

    inet_pton(AF_INET, server_conf.mgroup, &mreq.imr_multiaddr);
    inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
    mreq.imr_ifindex = if_nametoindex("ens33");

    serversd = socket(AF_INET, SOCK_DGRAM, 0);
    if(serversd < 0){
        syslog(LOG_ERR, "socket():%s", strerror(errno));
        exit(1);
    }

    if(setsockopt(serversd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0){
        syslog(LOG_ERR, "setsockopt(IP_MULTICAST_IF):%s", strerror(errno));
        exit(1);
    }

    sndaddr.sin_family = AF_INET;
    sndaddr.sin_port = htons(atoi(server_conf.rcvport));
    inet_pton(AF_INET, server_conf.mgroup, &sndaddr.sin_addr);

    return ;
}


int main(int argc, char **argv){
    int c;
    struct sigaction sa;

    sa.sa_handler = daemon_exit;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaddset(&sa.sa_mask, SIGTERM);
    
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    openlog("netradio", LOG_PID | LOG_PERROR, LOG_DAEMON);

    /*命令行分析*/
    while(1){
        c = getopt(argc, argv, "M:P:FD:I:H");
        if(c < 0)
            break;
        switch(c){
            case 'M':
                server_conf.mgroup = optarg;
                break;
            case 'P':
                server_conf.rcvport = optarg;
                break;
            case 'F':
                server_conf.runmode = RUN_FOREGROUND;
                break;
            case 'D':
                server_conf.media_dir = optarg;
                break;
            case 'I':
                server_conf.ifname = optarg;
                break;
            case 'H':
                print_help();
                exit(0);
                break;
            default:
                abort();
                break;
        }
    }

 
    /*守护进程的实现*/
    if(server_conf.runmode == RUN_DAEMON){
        if(daemonize() != 0)
            syslog(LOG_ERR, "daemonize() failed.");
            exit(1);
    }else if(server_conf.runmode == RUN_FOREGROUND){

    }else{
        syslog(LOG_ERR, "EINVAL server_conf.runmode.");
        exit(1);
    }
    /*SOCKET初始化*/
    socket_init();

    /*获取频道信息*/
    int list_size;
    int err;
    err = mlib_getchnlist(&list, &list_size);
    if(err){
        syslog(LOG_ERR, "mlib_getchnlist():%s", strerror(errno));
        exit(1);
    }
    /*创建节目单线程*/
    err = thr_list_create(list, list_size);
    if(err)
        exit(1);

    /*创建频道线程*/
    int i;
    for(i = 0; i < list_size; i++){
        err = thr_channel_create(list + i);
        if(err){
            fprintf(stderr, "thr_channel_create():%s\n", strerror(err));
            exit(1);
        }
    }
    
    syslog(LOG_DEBUG, "%d channel threads created.", i);
    while(1)
        pause;

    exit(0);
}