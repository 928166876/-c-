#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <errno.h>

#include "../include/proto.h"
#include "client.h"
/*
*   -M  --mgroup    指定多播组
*   -P  --port      指定接收端口
*   -p  --player    指定播放器   
*   -H  --help      显示帮助
**/



struct client_conf_st client_conf = {
    .recvport = DEFAULT_RCVPORT,\
    .mgroup = DEFAULT_MGROUP,\
    .player_cmd = DEFAULT_PLAYERCMD
};


static void print_help(void){
    printf("-P --port specify receive port\n");
    printf("-M --mgroup specify multicast group\n");
    printf("-p --port specify player\n");
    printf("-H --help show help\n");
}

static int writen(int fd, const void *buf, size_t len){
    int ret = 0;
    int pos = 0;
    while(len > 0){
        ret = write(fd, buf+pos, len);
        if(ret < 0){
            if(ret == EINTR)
                continue;
            perror("write()");
            return -1;
        }
        len -= ret;
        pos += ret;
    }
    return 0;
}



int main(int argc, char **argv){
    /*初始化的级别*/
    // 默认值、配置文件、环境变量、命令行参数
    int index = 0;
    int c;
    int sd;
    int len;
    int ret, chosenid;
    int val = 1;
    int pd[2];
    pid_t pid;
    struct ip_mreqn mreq;
    struct sockaddr_in laddr, raddr;
    struct sockaddr_in server_raddr;
    struct msg_listentry_st *pos;
    socklen_t sevreraddr_len, raddr_len;
    struct option argarr[] = {{"port", 1, NULL, 'P'},
                             {"mgroup", 1, NULL, 'M'},
                             {"help", 0, NULL, 'H'}, 
                             {"player", 1, NULL, 'p'}, 
                             {NULL, 0, NULL, 0}};
    while(1){
        c = getopt_long(argc, argv, "P:M:p:H", argarr, &index);
        if(c < 0)
            break;
        switch(c){
            case 'P':
                client_conf.recvport = optarg;
                break;
            case 'M':
                client_conf.mgroup = optarg;
                break;
            case 'p':
                client_conf.player_cmd = optarg;
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

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("socket()");
        exit(1);
    }

    inet_pton(AF_INET, client_conf.mgroup, &mreq.imr_multiaddr);
    /*if error*/
    inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
    mreq.imr_ifindex =if_nametoindex("ens33");
    if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))< 0){
        perror("setsockopt()");
        exit(1);
    }

    //improve efficiency
    if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof(val)) < 0){
        perror("setsockopt()");
        exit(1);
    }

    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(client_conf.recvport));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);
    if(bind(sd, (void *)&laddr, sizeof(laddr)) < 0){
        perror("bind()");
        exit(1);
    }

    if(pipe(pd) < 0){
        perror("pipe()");
        exit(1);
    }
    
    pid = fork();
    if(pid < 0){
        perror("fork()");
        exit(1);
    }
    if(pid == 0){
        //子进程：调用解码器
        close(sd);
        close(pd[1]); // 0->read, 1->write
        dup2(pd[0], 0);
        if(pd[0] > 0)
            close(pd[0]);
        execl("/bin/sh", "sh", "-c", client_conf.player_cmd, NULL);
        perror("execl()");
        exit(1);
    }else{

        struct msg_list_st *msg_list;
        msg_list = malloc(MSG_LIST_MAX);
        if(msg_list == NULL){
            perror("malloc()");
            exit(1);
        }

        sevreraddr_len = sizeof(server_raddr);
        while(1){
            len = recvfrom(sd, msg_list, MSG_LIST_MAX, 0, (void *)&server_raddr, &sevreraddr_len);
            if(len < sizeof(struct msg_list_st)){
                fprintf(stderr, "massage is too short.\n");
                continue;
            }
            if(msg_list->chnid != LISTCHNID){
                fprintf(stderr, "current chnid:%d\n", msg_list->chnid);
                fprintf(stderr, "chnid is not match.\n");
                continue;
            }
            break;
        }
        for(pos = msg_list->entry; (char *)pos < ((char *)msg_list + len); pos = (void *)(((char *)pos) + ntohs(pos->len))){
            printf("channel:%d %s", pos->chnid, pos->desc);
        }

        free(msg_list);
        puts("Please choose channel:");
        while(ret < 1){
            ret = scanf("%d",&chosenid);
            if(ret != 1)
                exit(1);
        }

        struct msg_channel_st *msg_channel;
        msg_channel = malloc(MSG_CHANNEL_MAX);
        if(msg_channel == NULL){
            perror("malloc()");
            exit(1);
        }

        raddr_len = sizeof(raddr);
        while(1){
            len = recvfrom(sd, msg_channel, MSG_CHANNEL_MAX, 0, (void *)&raddr, &raddr_len);
            if(len < sizeof(struct msg_channel_st)){
                fprintf(stderr, "message is too short.\n");
                continue;
            }

            if(raddr.sin_addr.s_addr != server_raddr.sin_addr.s_addr || raddr.sin_port != server_raddr.sin_port){
                fprintf(stderr, "addr or port not match.\n");   
                continue;
            }

            if(msg_channel->chnid == chosenid){
                fprintf(stderr, "accpeted msg from chnid:%d \n", msg_channel->chnid);
                if(writen(pd[1], msg_channel->data, len - sizeof(chnid_t)) < 0){
                    exit(1);
                }
            }

        }

        free(msg_channel);
        close(sd);
        //父进程
        exit(0);

    }
}