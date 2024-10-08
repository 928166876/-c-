#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static jmp_buf save;

static void d(void){
    printf("%s begin!\n", __FUNCTION__);
    longjmp(save, 0);   //开始往回跳， 返回值为6,值为0会被系统修改
    printf("%s end!\n", __FUNCTION__);
}

static void c(void){
    printf("%s begin!\n", __FUNCTION__);
    d();
    printf("%s end!\n", __FUNCTION__);
}

static void b(void){
    printf("%s begin!\n", __FUNCTION__);
    c();
    printf("%s end!\n", __FUNCTION__);
}

static void a(void){
    int ret = setjmp(save); //设置跳转点，安全跳转
    if(ret == 0){           
        printf("%s begin!\n", __FUNCTION__);
        b();
        printf("%s end!\n", __FUNCTION__);
    }else{
        printf("code %d return\n", ret);  
    }
}

int main(void){

    printf("%s begin!\n", __FUNCTION__);
    a();
    printf("%s end!\n", __FUNCTION__);

    exit(0);
}