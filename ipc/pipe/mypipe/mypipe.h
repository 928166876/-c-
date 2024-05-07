#ifndef MYPIPE_H__
#define MYPIPE_H__

#include <sys/types.h>
#define PIPESIZE 1024
#define PIPE_READER 0x00000001UL //reader
#define PIPE_WRITER 0x00000002UL //writer

typedef void mypipe_t;


mypipe_t  *mypipe_init();

int mypipe_read(mypipe_t *, void *buf, size_t size);

int mypipe_write(mypipe_t *, const void *buf, size_t size);

int mypipe_destory(mypipe_t *);



#endif