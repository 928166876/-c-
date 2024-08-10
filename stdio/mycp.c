#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv){

    if(argc < 3){       //判断参数个数
        fprintf(stderr, "Usage:%s <src_file_name> <dest_file_name>\n", argv[0]);
        exit(1);
    }
    FILE *fps;      //定义两个文件指针
    FILE *fpd;      //一个用来读，一个用来写

    fps = fopen(argv[1], "r");    //打开源文件
    if(fps == NULL){
        perror("fopen()");
        exit(1);
    }
    fpd = fopen(argv[2], "w");  //打开目标文件
    if(fpd == NULL){
        fclose(fps);        //打开失败则 关闭源文件后续可以用atexit()函数
        perror("fopen()");
        exit(1);
    }

    int res = 0;
    while (1){
        res = fgetc(fps);    //读取一个字符
        if(res == EOF){      //判断是否到文件末尾
            break;
        }
        fputc(res, fpd);     //写入目标文件
    }

    fclose(fps);        //关闭文件
    fclose(fpd);

    exit(0);
}