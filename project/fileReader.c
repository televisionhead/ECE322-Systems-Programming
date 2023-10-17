#include <stdio.h>

#define STRLEN 20
#define PDEBUG 1
#define DPRINT(fmt, ...) do { if (PDEBUG) fprintf(stderr, "%s:%d:%s(): "fmt,\
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while (0)

int main(int argc, char* argv[]) {
        if(argc != 2) {
                DPRINT("Usage: fileReader <filename>\n");
                return -1;
        }

        FILE* in;
        if((in = fopen(argv[1], "r")) == NULL) {
                DPRINT("Error: fopen returned NULL\n");
                return -1;
        }

        char str[STRLEN];
        while(fscanf(in, "%s", str) != EOF) printf("%s\n", str);

        fclose(in);
    return 0;
}