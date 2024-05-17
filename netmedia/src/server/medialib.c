#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <stdint.h>
#include <sys/types.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "server_conf.h"
#include "mytbf.h"
#include "medialib.h"
#include "../include/proto.h"

#define PATHSIZE        1024
#define LINEBUFSIZE     1024
#define MP3_BITRATE     320 * 1024

struct channel_context_st{
    chnid_t chnid;
    char *desc;
    glob_t mpg3glob;
    int pos;
    int fd;
    off_t offset;
    mytbf_t *tbf;
};

static struct channel_context_st channel[MAXCHNID + 1];

static struct channel_context_st *path2entry(const char *path){
    FILE *fp;
    syslog(LOG_INFO, "current path: %s", path);
    char pathstr[PATHSIZE] = {'\0'};
    struct channel_context_st *me;
    char linebuf[LINEBUFSIZE];
    static chnid_t currid = MINCHNID;

    //检测目录合法性
    strcat(pathstr, path);
    strcat(pathstr, "/desc.text");
    fp = fopen(pathstr, "r");
    if(fp == NULL){
        syslog(LOG_INFO, "%s is not a channel dir (can not find desc.text)", path);
#if DEBUG
        printf("path:%s\n", path);
#endif
        return NULL;

    }
    if(fgets(linebuf, LINEBUFSIZE, fp) == NULL){
        syslog(LOG_INFO, "%s desc.text is empty.", path);
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    //初始化上下文
    me = malloc(sizeof(*me));
    if(me == NULL){
        syslog(LOG_ERR, "malloc():%s", strerror(errno));
        free(me);
        return NULL;
    }

    me->tbf = mytbf_init(MP3_BITRATE/8, MP3_BITRATE/8 * 5);
    if(me->tbf == NULL){
        syslog(LOG_ERR, "mytbf_init():%s", strerror(errno));
        free(me);
        return NULL;
    }


    //初始化频道
    me->desc = strdup(linebuf);
    strncpy(pathstr, path, PATHSIZE);
    strncat(pathstr, "/*.mp3", PATHSIZE-1);
    if(glob(pathstr, 0, NULL, &me->mpg3glob) != 0){
        currid++;
        syslog(LOG_ERR, "%s is not a channel dir(can not find mp3 files)", path);
        free(me);
        return NULL;
    }

    me->pos = 0;
    me->offset = 0;
    me->fd = open(me->mpg3glob.gl_pathv[me->pos], O_RDONLY);
    if(me->fd < 0){
        syslog(LOG_WARNING, "%s open failed.", me->mpg3glob.gl_pathv[me->pos]);
        free(me);
        return NULL;
    }

    me->chnid = currid;
    currid++;
    return me;

}


int mlib_getchnlist(struct mlib_listentry_st **result, int *resnum){
    int num = 0;
    glob_t globres;
    char path[PATHSIZE];
    struct mlib_listentry_st *ptr;
    struct channel_context_st *res;
    for(int i = 0; i < MAXCHNID; i++){
        channel[i].chnid = -1;
    }

    snprintf(path, PATHSIZE, "%s/*", server_conf.media_dir);
#if DEBUG
    syslog(LOG_ERR, "zy:current path:%s", path);
#endif
    if (glob(path, 0, NULL, &globres)){
        return -1;
    }

#if DEBUG
    printf("globres.gl_pathc:%d\n", globres.gl_pathc);
    printf("globres.gl_pathv[0]:%s\n", globres.gl_pathv[0]);
    printf("globres.gl_pathv[1]:%s\n", globres.gl_pathv[1]);
#endif

    ptr = malloc(sizeof(struct mlib_listentry_st) * globres.gl_pathc);
    if(ptr == NULL){
        syslog(LOG_ERR, "malloc() error.");
        exit(1);
    }

    for(int i = 0; i < globres.gl_pathc; i++){
        //globres.gl_pathv[i] -> "/var/media/ch1"
        res = path2entry(globres.gl_pathv[i]);
        if(res != NULL){
            syslog(LOG_ERR, "path2entry() return : %d %s.", res->chnid, res->desc);
            memcpy(channel + res->chnid, res, sizeof(*res));
            ptr[num].chnid = res->chnid;
            ptr[num].desc = strdup(res->desc);
            num++;
        }
#if DEBUG
        printf("num:%d\n", num);
#endif
    }

    *result = realloc(ptr, sizeof(struct mlib_listentry_st) * num);
    if (*result == NULL){
        syslog(LOG_ERR, "realloc() failed.");
    }

    *resnum = num;
    return 0;
}

int mlib_freechnlist(struct mlib_listentry_st *ptr){
    free(ptr);
    return 0;
}


static int open_next(chnid_t chnid){
    for(int i = 0; i < channel[chnid].mpg3glob.gl_pathc; i++){
        channel[chnid].pos++;
        if(channel[chnid].pos == channel[chnid].mpg3glob.gl_pathc){
            channel[chnid].pos = 0;
            break;
        }
        close(channel[chnid].fd);

        //尝试打开新的文件
        channel[chnid].fd = open(channel[chnid].mpg3glob.gl_pathv[channel[chnid].pos], O_RDONLY);
        if (channel[chnid].fd < 0){
            syslog(LOG_WARNING, "open(%s):%s", channel[chnid].mpg3glob.gl_pathv[channel[chnid].pos], strerror(errno));
        }else{
            channel[chnid].offset = 0;
            return 0;
        }
    }
    syslog(LOG_ERR, "None of mp3 in channel %d is available", chnid);
    return -1;
}

ssize_t mlib_readchn(chnid_t chnid, void *buf, size_t size){
    int len;
    int tbfsize;

    tbfsize = mytbf_fetchtoken(channel[chnid].tbf, size);

    while(1){
        len = pread(channel[chnid].fd, buf, tbfsize, channel[chnid].offset);
        if(len < 0){
            syslog(LOG_WARNING, "media file %s pread(): %s", channel[chnid].mpg3glob.gl_pathv[channel[chnid].pos], strerror(errno));
            open_next(chnid);
        }else if(len == 0){
            syslog(LOG_DEBUG, "media %s file is over", channel[chnid].mpg3glob.gl_pathv[channel[chnid].pos]);
            open_next(chnid);
        }else{
            channel[chnid].offset += len;
            break;
        }
    }
    if(tbfsize - len > 0){
        mytbf_returntoken(channel[chnid].tbf, tbfsize - len);
    }
    return len;

}