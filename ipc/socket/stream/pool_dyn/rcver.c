#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <errno.h>

#include "proto.h"

#define SIG_NOTIFY      SIGUSR2

#define MINSPACESERVER  5
#define MAXSPACESERVER  10
#define MAXCLIENTS      20
#define IPSTRSIZE       40
#define BUFSIZE         80

enum{
    STATE_IDLE=0,
    STATA_BUSY
};

struct server_st{
    pid_t pid;
    int state;
};


static struct server_st *serverpool;
static int idle_count = 0, busy_count = 0;

static int sd;

static void handle(int sig){
    return;
}

static void server_job(int pos){
    int newsd;
    int ppid;
    int len;
    struct sockaddr_in raddr;
    socklen_t raddr_len;
    char ipstr[IPSTRSIZE];
    char buf[BUFSIZE];

    ppid = getppid();
    raddr_len = sizeof(raddr);
    while(1){
        serverpool[pos].state = STATE_IDLE;
        kill(ppid, SIG_NOTIFY);

        newsd = accept(sd, (void *)&raddr, &raddr_len);
        if(newsd < 0){
            if(errno != EINTR || errno != EAGAIN){
                perror("accept()");
                exit(1);
            }
        }

        serverpool[pos].state = STATA_BUSY;
        kill(ppid, SIG_NOTIFY);
        inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);

        len = sprintf(buf, FMT_STAMP, (long long)time(NULL));
        if(send(newsd, buf, len, 0) < 0){
            perror("send()");
            exit(1);
        }
        close(newsd);
        sleep(5);
    }
}

static int add_one_server(){
    pid_t pid;
    int slot;

    if(idle_count + busy_count >= MAXCLIENTS){
        return -1;
    }

    for(slot = 0; slot < MAXCLIENTS; slot++){
        if(serverpool[slot].pid == -1)
            break;
    }

    serverpool[slot].state = STATE_IDLE;
    pid = fork();
    if(pid < 0){
        perror("fork()");
        exit(1);
    }
    if(pid == 0){
        server_job(slot);
        exit(1);
    }else{
        serverpool[slot].pid = pid;
        idle_count++;
    }
    return 0;
}

static int del_one_server(){
    if(idle_count == 0){
        return -1;
    }

    for(int i = 0; i < MAXCLIENTS; i++){
        if(serverpool[i].pid != -1 && serverpool[i].state == STATE_IDLE){
            kill(serverpool[i].pid, SIGTERM);
            serverpool[i].pid = -1;
            idle_count--;
            break;
        }
    }
    return 0;

}

static void scan_pool(){
    int idle = 0, busy = 0;
    for(int i = 0; i < MAXCLIENTS; i++){
        if(serverpool[i].pid == -1)
            continue;
        if(kill(serverpool[i].pid, 0)){
            serverpool[i].pid = -1;
            continue;
        }

        if(serverpool[i].state == STATE_IDLE){
            idle++;
        }else if(serverpool[i].state == STATA_BUSY){
            busy++;
        }else{
            fprintf(stderr, "unknow state\n");
            abort();
        }
    }
    idle_count = idle;
    busy_count = busy;

}

int main(void){

    struct sockaddr_in laddr;
    sigset_t set, oset;
    struct sigaction sa,osa;


    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDWAIT;
    sigaction(SIGCLD, &sa, &osa);


    sa.sa_handler = handle;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIG_NOTIFY, &sa, &osa);

    //屏蔽信号
    sigemptyset(&set);
    sigaddset(&set, SIG_NOTIFY);
    sigprocmask(SIG_BLOCK, &set, &oset);

    serverpool = mmap(NULL, sizeof(struct server_st) * MAXCLIENTS, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if(serverpool == MAP_FAILED){
        perror("mmap()");
        exit(1);
    }

    for(int i = 0; i < MAXCLIENTS; i++){
        serverpool[i].pid = -1;
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd < 0){
        perror("socket()");
        exit(1);
    }

    int  val = 1;
    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0){
        perror("setsockopt()");
        exit(1);
    }

    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);
    if(bind(sd, (void *)&laddr, sizeof(laddr)) < 0){
        perror("bind()");
        exit(1);
    }

    if(listen(sd, 200) < 0){
        perror("listen()");
        exit(1);
    }

    for(int i = 0; i < MINSPACESERVER; i++){
        add_one_server();
    }

    while(1){
        sigsuspend(&oset);

        scan_pool();

        if(idle_count > MAXSPACESERVER){
            for(int i = 0; i < (idle_count - MAXSPACESERVER); i++){
                del_one_server();
            }
        }

        if(idle_count < MINSPACESERVER){
            for(int i =0; i < (MINSPACESERVER - idle_count); i++){
                add_one_server();
            }
        }

        for(int i = 0; i < MAXCLIENTS; i++){
            if(serverpool[i].pid == -1){
                putchar(' ');
            }else if(serverpool[i].state == STATE_IDLE)
                putchar('.');
            else if(serverpool[i].state == STATA_BUSY)
                putchar('X');
        }
        printf("\n");
    }

    close(sd);
    sigprocmask(SIG_SETMASK, &oset, NULL);

    exit(0);
}
