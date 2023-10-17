#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

void createpipe(int* buf) {
	if(pipe(buf) == -1) {
		fprintf(stderr, "pipe returned -1 in createpipe: %s\n", strerror(errno));
		exit(-1);
	}
}

int createchild() {
	int pid;
	if((pid = fork()) == -1) {
		fprintf(stderr, "fork returned -1 in createchild: %s\n", strerror(errno));
		exit(-1);
	} else return pid;
}

void writetopipe(int* pipe, char* from, int amt) {
	if(write(pipe[1], from, amt) == -1) {
		fprintf(stderr, "write returned -1 in writetopipe: %s\n", strerror(errno));
		exit(-1);
	}
}

int read1frompipe(int* pipe, char* into) {
	int ret;
	if((ret = read(pipe[0], into, 1)) == -1) {
		fprintf(stderr, "read returned -1 in readchar: %s\n", strerror(errno));
		exit(-1);
	} else return ret;
}

int readXfrompipe(int* pipe, char* into, int amt) {
	char c;
	int count = 0;
	while(read1frompipe(pipe, &c)) {
		into[count] = c;
		count++;
		c = 0;
		if(count == amt) break;
	}
	return count;
}

void oper1(char* in) {
	char tmp[4] = {in[0], in[1], in[2], in[3]};
	in[0] = (tmp[0] | tmp[1]) & (tmp[2] | tmp[3]);
	in[1] = tmp[2];
	in[2] = tmp[1];
	in[3] = tmp[1] & tmp[2] & tmp[3];
}

unsigned int oper2(char* in) {
	unsigned int out = 0;
	out = out | (in[1] << 24);
	out = out | (in[3] << 16);
	out = out | (in[2] << 8);
	out = out | in[0];
	return out;
}

void printchangewrite(char* in, int* pipe) {
	printf("%s\n", in);
	oper1(in);
	writetopipe(pipe, in, 4);
}

void child1code(int* rpipe, int* wpipe) {
	close(rpipe[1]);
	char in[4];
	int numread;
	while((numread = readXfrompipe(rpipe, in, 4)) == 4) printchangewrite(in, wpipe);
	close(rpipe[0]);
	if(numread) {
		for(int i = numread; i < 4; i++) in[i] = 'z';
		printchangewrite(in, wpipe);
	}
	close(wpipe[0]);
	close(wpipe[1]);
	exit(0);
}

void child2code(int* pipe) {
	close(pipe[1]);
	unsigned int inter, total = 0; 
	char in[4];
	while(readXfrompipe(pipe, in, 4)) {
		printf("intermediate: %u\n", inter = oper2(in));
		total += inter;
	}
	close(pipe[0]);	
	printf("\n★final result: %u\n", total % ((unsigned long)UINT_MAX + 1));
	exit(0);
}

void parentcode(int* pipe1, int* pipe2, int child1, int child2) {
	close(pipe1[0]);
	close(pipe2[0]);
	close(pipe2[1]);
	char c;
	while((c = getchar()) != EOF) writetopipe(pipe1, &c, 1);
	close(pipe1[1]);
	waitpid(child1, NULL, 0);
	waitpid(child2, NULL, 0);
}

int main() {
	int pipe2[2];
	createpipe(pipe2);	
	int child2 = createchild();
	if(!child2) child2code(pipe2);

	int pipe1[2];
	createpipe(pipe1);
	int child1 = createchild();
	if(!child1) child1code(pipe1, pipe2);	

	parentcode(pipe1, pipe2, child1, child2);

	return 0;
}