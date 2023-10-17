#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define PDEBUG 1
#define DPRINT(fmt, ...) do { if (PDEBUG) fprintf(stderr, "%s:%d:%s(): "fmt,\
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while (0)

int* createpipe() {
        int* arr = (int*)malloc(sizeof(int)*2);
        if(pipe(arr) == -1) {
                DPRINT("Error: pipe returned -1\n");
                exit(-1);
        }
        return arr;
}

int createchild() {
        int pid;
        if((pid = fork()) == -1) {
                DPRINT("Error: fork returned -1\n");
                exit(-1);
        }
        return pid;
}

void createdupe(int oldfd, int newfd) {
        if(dup2(oldfd, newfd) == -1) {
                DPRINT("Error: dup2 returned -1\n");
                exit(-1);
        }
}

void runprog(char* path, char* name, char* arg) {
        if(execlp(path, name, arg, NULL) == -1) {
                DPRINT("Error: execlp returned -1\n");
                exit(-1);
        }
}

int main(int argc, char* argv[]) {
        //determine if user specified an output file
        char* output;
        if(argc == 9) output = argv[8];
        else if(argc == 8) output = NULL;
        else {
                DPRINT("Invalid arguments!\n");
                exit(-1);
        }

        //initialize pipe/children array
        int* pipes[7];
        int children[11];
        for(int i = 0; i < 7; i++) {
                pipes[i] = createpipe();
        }

        //4 fileReader children
        for(int i = 0; i < 4; i++) {
                children[i] = createchild();
                if(!children[i]) {
                        createdupe(pipes[i][1], STDOUT_FILENO);
                        runprog(argv[1], "fileReader", argv[i+2]);
                        exit(0);
                }
        }
        for(int i = 0; i < 4; i++) close(pipes[i][1]);

        //4 aSorter children
        for(int i = 4; i < 8; i++) {
                children[i] = createchild();
                if(!children[i]) {
                        if(i < 6) {
                                createdupe(pipes[i-4][0], STDIN_FILENO);
                                createdupe(pipes[4][1], STDOUT_FILENO);
                                runprog(argv[6], "aSorter", "16");
                        } else {
                                createdupe(pipes[i-4][0], STDIN_FILENO);
                                createdupe(pipes[5][1], STDOUT_FILENO);
                                runprog(argv[6], "aSorter", "42");
                        }
                        exit(0);
                }
        }
        for(int i = 4; i < 6; i++) close(pipes[i][1]);

        //2 intermediate merger children
        for(int i = 8; i < 10; i++) {
                children[i] = createchild();
                if(!children[i]) {
                        createdupe(pipes[i-4][0], STDIN_FILENO);
                        createdupe(pipes[6][1], STDOUT_FILENO);
                        runprog(argv[7], "aMerger", NULL);
                        exit(0);
                }
        }
        for(int i = 0; i < 4; i++) close(pipes[i][0]);
        close(pipes[6][1]);

        //final merge child
        children[10] = createchild();
        if(!children[10]) {
                createdupe(pipes[6][0], STDIN_FILENO);
                runprog(argv[7], "theMerger", output);
                exit(0);
        }

        //wait for all children to finish
        for(int i = 0; i < 11; i++) waitpid(children[i], NULL, 0);

        //free the memory allocated for pipes
        for(int i = 0; i < 7; i++) free(pipes[i]);

        return 0;
}