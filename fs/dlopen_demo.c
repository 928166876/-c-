#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <gnu/lib-names.h>

int main(void)
{   
    double (*cosine)(double);
    void *handle;
    char *errorstr;

    handle =  dlopen(LIBM_SO, RTLD_LAZY);
    if(!handle){
        fprintf(stderr, "%s\n", dlerror);
        exit(1);
    }
    dlerror();
    cosine = (double (*) (double)) dlsym(handle, "cos");
    errorstr = dlerror();
    if(errorstr != NULL){
        fprintf(stderr, "%s\n", errorstr);
        exit(1);
    }
    printf("%f\n", (*cosine)(2.0));
    dlclose(handle);
    exit(0);
    
}