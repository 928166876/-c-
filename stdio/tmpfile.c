#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
    FILE *tmpfp;
    tmpfp = tmpfile();   //创建一个临时文件(原子化操作创建并打开)

    const char *buf = "2024-04-12";
    int size = strlen(buf); //获取字符串长度
    // while(*(buf+size) != '\0'){
    //     size++;
    // }
    printf("%d\n", size);

    fwrite(buf, 1, size, tmpfp);   //写入数据
    fseek(tmpfp, 0, SEEK_SET); //将文件指针移动到文件开头
    
    FILE *fp;
    fp = fopen("tmp", "w"); 
    char desbuf[size]; //定义一个缓冲区
    int n = fread(desbuf, 1, size, tmpfp);  //读取数据
    fwrite(desbuf, 1, n, fp);       //写入数据

    fclose(tmpfp);
    fclose(fp);

    exit(0);
}