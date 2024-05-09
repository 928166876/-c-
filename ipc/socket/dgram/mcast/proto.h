#ifndef PROTO_H__
#define PROTO_H__

#include <stdint.h>

#define MGROUB  "224.2.2.2"
#define NAMESIZE 40
#define SERVERPORT  "1989"

struct msg_st{
    uint8_t name[NAMESIZE];
    uint32_t math;
    uint32_t chinese;
}__attribute__((packed));


#endif