#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
    FILE *tmpfp;
    tmpfp = tmpfile();

    const char *buf = "2024-04-12";
    int size = strlen(buf);
    // while(*(buf+size) != '\0'){
    //     size++;
    // }
    printf("%d\n", size);

    fwrite(buf, 1, size, tmpfp);
    fseek(tmpfp, 0, SEEK_SET);
    
    FILE *fp;
    fp = fopen("tmp", "w"); 
    char *desbuf[size];
    int n = fread(desbuf, 1, size, tmpfp);
    fwrite(desbuf, 1, n, fp);

    fclose(tmpfp);
    fclose(fp);

    exit(0);
}