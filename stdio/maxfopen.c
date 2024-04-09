#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(){
	int count;
	FILE* fp = NULL;
	while(1){
		fp = fopen("tmp", "r");
		if(fp == NULL){
			perror("fopen()");
			break;
		}
		count++;
	}
	printf("count = %d\n", count);
	exit(0);
}

