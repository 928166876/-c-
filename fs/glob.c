#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <glob.h>


#define PAT "/etc/*.conf"
#if 0
static int errfunc_(const char *errpath, int eerrno){
    puts(errpath);
    fprintf(stderr, "ERROR MSG:%s\n", strerror(eerrno));
    return 0;    
}
#endif

int main(){
    glob_t globres;

    int err = glob(PAT, 0,NULL, &globres);
    if(err){
        fprintf(stderr,"Error code = %d\n", err);
        exit(1);
    }

    for(int i = 0; globres.gl_pathv[i] != NULL ; i++){
        puts(globres.gl_pathv[i]);
    }

    globfree(&globres);
    exit(0);
}