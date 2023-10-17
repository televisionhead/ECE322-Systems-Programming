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

int main(int argc, char* argv[]) {
        char* child5arg;

        if(argc == 6) {
                child5arg = NULL;
        }
        else if(argc == 7) {
                child5arg = argv[6];
        }
        else {
                DPRINT("Usage: myStarter <child1/2 program> <child1 argument> <child2 argument> <child3/4 program> <child5 program> <child5 argument>\n");
                return -1;
        }

        int* pipe1 = createpipe();
        int child1 = createchild();

        if(!child1) {
                close(pipe1[0]);

                if(dup2(pipe1[1], STDOUT_FILENO) == -1) {
                        DPRINT("Error: dup2 returned -1 in child1\n");
                        exit(-1);
                }

                if(execlp(argv[1], "fileReader1", argv[2], NULL) == -1) {
                        DPRINT("Error: execlp returned -1 in child1\n");
                        exit(-1);
                }

                exit(0);
        }

        int* pipe2 = createpipe();
        int child2 = createchild();

        if(!child2) {
                close(pipe1[0]);
                close(pipe1[1]);
                close(pipe2[0]);

                if(dup2(pipe2[1], STDOUT_FILENO) == -1) {
                        DPRINT("Error: dup2 returned -1 in child2\n");
                        exit(-1);
                }

                if(execlp(argv[1], "fileReader2", argv[3], NULL) == -1) {
                        DPRINT("Error: execlp returned -1 in child2\n");
                        exit(-1);
                }

                exit(0);
        }

        int* pipe3 = createpipe();
        int child3 = createchild();

        if(!child3) {
                close(pipe1[1]);
                //close(pipe2[0]);
                close(pipe2[1]);
                close(pipe3[0]);

                if(dup2(pipe1[0], STDIN_FILENO) == -1 || dup2(pipe3[1], STDOUT_FILENO) == -1) {
                        DPRINT("Error: dup2 returned -1 in child3\n");
                        exit(-1);
                }

                if(execlp(argv[4], "aSorter1", "42", NULL) == -1) {
                        DPRINT("Error: execlp returned -1 in child3\n");
                        exit(-1);
                }
                exit(0);
        }

        int child4 = createchild();

        if(!child4) {
                //close(pipe1[0]);
                close(pipe1[1]);
                close(pipe2[1]);
                close(pipe3[0]);

                if(dup2(pipe2[0], STDIN_FILENO) == -1 || dup2(pipe3[1], STDOUT_FILENO) == -1) {
                        DPRINT("Error: dup2 returned -1 in child4\n");
                        exit(-1);
                }

                if(execlp(argv[4], "aSorter2", "16", NULL) == -1) {
                        DPRINT("Error: execlp returned -1 in child4\n");
                        exit(-1);
                }

                exit(0);
        }

        int child5 = createchild();

        if(!child5) {
                close(pipe1[0]);
                close(pipe1[1]);
                close(pipe2[0]);
                close(pipe2[1]);
                close(pipe3[1]);

                if(dup2(pipe3[0], STDIN_FILENO) == -1) {
                        DPRINT("Error: dup2 returned -1 in child5\n");
                        exit(-1);
                }

                if(execlp(argv[5], "theMerger", child5arg, NULL) == -1) {
                        DPRINT("Error: execlp returned -1 in child5\n");
                        exit(-1);
                }

                exit(0);
        }

        waitpid(child1, NULL, 0);
        close(pipe1[1]);

        waitpid(child2, NULL, 0);
        close(pipe2[1]);

        waitpid(child3, NULL, 0);
        close(pipe1[0]);
        close(pipe3[1]);

        waitpid(child4, NULL, 0);
        waitpid(child5, NULL, 0);

        free(pipe1);
        free(pipe2);
        free(pipe3);

        return 0;
}