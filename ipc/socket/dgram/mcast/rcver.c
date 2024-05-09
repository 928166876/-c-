#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>

#include "proto.h"

#define IPSTRSIZE   40



int main(void){
    int sd;
    struct ip_mreqn mreq;
    socklen_t raddr_len;
    struct sockaddr_in laddr, raddr;
    struct msg_st rbuf;
    char ipstr[IPSTRSIZE];
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("socket()");
        exit(1);
    }

    inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
    inet_pton(AF_INET, MGROUB, &mreq.imr_multiaddr);
    mreq.imr_ifindex = if_nametoindex("ens33");
    if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0){
        perror("setsockopt()");
        exit(1);
    }
    // if (setsockopt(sd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0){
    //     perror("setsockopt()");
    //     exit(1);
    // }

    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);
    if (bind(sd, (void *)&laddr, sizeof(laddr)) < 0){
        perror("bind()");
        exit(1);
    }
    /*!!!*/
    raddr_len = sizeof(raddr);
    while(1){
        recvfrom(sd, &rbuf, sizeof(rbuf), 0, (void *)&raddr, &raddr_len);

        inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);
        printf("mesage form %s:%d\n", ipstr, ntohs(raddr.sin_port));
        printf("name: %s\n", rbuf.name);
        printf("math: %d\n", ntohl(rbuf.math));
        printf("chinese: %d\n", ntohl(rbuf.chinese));
        fflush(NULL);
    }

    close(sd);

    exit(0);
}