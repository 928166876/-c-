#ifndef PROTO_H__
#define PROTO_H__

#include <stdint.h>

#define NAMESIZE (512 - 8 - 8)
#define SERVERPORT  "1989"

struct msg_st{
    uint32_t math;
    uint32_t chinese;
    uint8_t name[1];
}__attribute__((packed));


#endif