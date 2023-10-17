#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRLEN 20
#define PDEBUG 1
#define DPRINT(fmt, ...) do { if (PDEBUG) fprintf(stderr, "%s:%d:%s(): "fmt,\
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while (0)

void bsort(char** arr, const int len) {
        for(int i = 0; i < len; i++) {
                for(int j = 0; j < (len-i-1); j++) {
                        if(strcmp(arr[j], arr[j+1]) > 0) {
                                char* tmp = arr[j];
                                arr[j] = arr[j+1];
                                arr[j+1] = tmp;
                        }
                }
        }
}

int main(int argc, char* argv[]) {
        if(argc != 2) {
                DPRINT("Usage: aSorter <number>\n");
                return -1;
        }

        int strsize = sizeof(char)*STRLEN;
        int currsize = strsize*10;
        int count = 0;
        char** strarr = (char**)malloc(currsize);
        char str[STRLEN];

        while(fscanf(stdin, "%s", str) != EOF) {
                if((count*strsize) > currsize) {
                        currsize += currsize;
                        strarr = (char**)realloc(strarr, currsize);
                }

                strarr[count] = strdup(str);
                count++;
        }

        bsort(strarr, count);

        for(int i = 0; i < count; i++) printf("%s%s\n", argv[1], strarr[i]);

        for(int i = 0; i < count; i++) free(strarr[i]);
        free(strarr);

        return 0;
}