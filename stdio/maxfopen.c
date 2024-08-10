#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(){
	int count;
	FILE* fp = NULL;
	while(1){
		fp = fopen("tmp", "r"); //打开文件
		if(fp == NULL){
			perror("fopen()");
			break;
		}
		count++; //计数
	}
	printf("count = %d\n", count); //输出计数
	exit(0);
}

