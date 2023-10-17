#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRLEN 20
#define PDEBUG 1
#define DPRINT(fmt, ...) do { if (PDEBUG) fprintf(stderr, "%s:%d:%s(): "fmt,\
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while (0)

int get_num(char* str) {
        char num[10];
        int j = 0;
        for(int i = 0; i < strlen(str); i++) {
                if(str[i] >= '0' && str[i] <= '9') {
                        num[j] = str[i];
                        j++;
                } else {
                        return atoi(num);
                }
        }
}

char** merge(char** arr1, char** arr2, int size1, int size2) {
        char** ret = (char**)malloc(sizeof(char*)*(size1+size2));
        int i = 0, count1 = 0, count2 = 0;
        while(count1 < size1 && count2 < size2) {
                char str1[10];
                char str2[10];
                int num1 = get_num(arr1[count1]);
                int num2 = get_num(arr2[count2]);
                sprintf(str1, "%d", num1);
                sprintf(str2, "%d", num2);
                char str4[10];
                char str5[10];
                strncpy(str4, &((arr1[count1])[strlen(str1)]), strlen(arr1[count1]) - strlen(str1));
                strncpy(str5, &((arr2[count2])[strlen(str2)]), strlen(arr2[count2]) - strlen(str2));
                if(strcmp(str4, str5) < 0) {
                        ret[i++] = strdup(arr1[count1++]);
                } else {
                        ret[i++] = strdup(arr2[count2++]);
                }
        }
        while(count1 < size1) {
                ret[i++] = strdup(arr1[count1++]);
        }
        while(count2 < size2) {
                ret[i++] = strdup(arr2[count2++]);
        }
        return ret;
}

void freearr(char** arr, int size) {
        for(int i = 0; i < size; i++) free(arr[i]);
        free(arr);
}

int main(int argc, char* argv[]) {
        FILE* out;

        if(argc == 1) out = stdout;
        else if(argc == 2) {
                if((out = fopen(argv[1], "w")) == NULL) {
                        DPRINT("Error: fopen returned NULL\n");
                        return -1;
                }
        }
        else {
                DPRINT("Usage: theMerger <filename>\n");
                return -1;
        }

        int strsize = sizeof(char)*STRLEN;
        int currsize = strsize*10;
        int count1 = 0;
        int count2 = 0;
        int num1 = -1;
        int num2 = -1;
        int curr;

        char** arr1 = (char**)malloc(currsize);
        char** arr2 = (char**)malloc(currsize);
        char str[STRLEN];

        while(scanf("%s", str) != EOF) {
                if((count1*strsize) > currsize || (count2*strsize) > currsize) {
                        currsize += currsize;
                        arr1 = (char**)realloc(arr1, currsize);
                        arr2 = (char**)realloc(arr2, currsize);
                }

                curr = get_num(str);

                if(num1 == -1) {
                        num1 = curr;
                        arr1[count1] = strdup(str);
                        count1++;
                }
                else if(num2 == -1 && curr != num1) {
                        num2 = curr;
                        arr2[count2] = strdup(str);
                        count2++;
                }
                else if(curr == num1) {
                        arr1[count1] = strdup(str);
                        count1++;
                }
                else if(curr == num2) {
                        arr2[count2] = strdup(str);
                        count2++;
                }
                else {
                        DPRINT("Error: invalid case in while loop!\n");
                        return -1;
                }
        }

        char** merged = merge(arr1, arr2, count1, count2);

        for(int i = 0; i < (count1 + count2); i++) fprintf(out, "%s\n", merged[i]);

        freearr(arr1, count1);
        freearr(arr2, count2);
        freearr(merged, count1 + count2);

        fclose(out);

        return 0;
}