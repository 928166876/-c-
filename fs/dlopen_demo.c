#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <gnu/lib-names.h>

int main(void)
{   
    double (*cosine)(double);
    void *handle;
    char *errorstr;

    handle =  dlopen(LIBM_SO, RTLD_LAZY);   //打开一个动态链接库文件
    if(!handle){
        fprintf(stderr, "%s\n", dlerror);
        exit(1);
    }   
    dlerror();      /* Clear any existing error */
    cosine = (double (*) (double)) dlsym(handle, "cos");    //obtain address of a symbol in a shared object or executable
    errorstr = dlerror();
    if(errorstr != NULL){
        fprintf(stderr, "%s\n", errorstr);
        exit(1);
    }
    printf("%f\n", (*cosine)(2.0));
    dlclose(handle);
    exit(0);
    
}