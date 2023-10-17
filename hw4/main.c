#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ALEN 10
#define LLEN 50

int* createpipe() {
	int* arr = (int*)malloc(sizeof(int)*2);
	if(pipe(arr) == -1) {
		fprintf(stderr, "Error: pipe returned -1\n");
		exit(-1);
	}
	return arr;
}

int createchild() {
	int pid;
	if((pid = fork()) == -1) {
		fprintf(stderr, "Error: fork returned -1\n");
		exit(-1);
	}
	return pid;
}

void bsort(char** arr, const int len) {
	for(int i = 0; i < len; i++) {
		for(int j = 0; j < (len-i-1); j++) {
			if (strcmp(arr[j], arr[j+1]) > 0) {
				char* tmp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = tmp;
			}
		}
	}
}

void childsort(int* pipe, char* fname) {
	close(pipe[1]);
	FILE* file;
	if(file = fdopen(pipe[0], "r")) {		
		int count = 0;
		int charsize = sizeof(char*);
		int currsize = charsize*ALEN;
		char** data = (char**)malloc(currsize);
		char line[LLEN];	
		while(fscanf(file, "%s", line) != EOF) {
			if((count*charsize) > currsize) {
				currsize += charsize*ALEN;
				data = (char**)realloc(data, currsize);
			}
			data[count] = strdup(line);
			count++;
		}
		fclose(file);
		close(pipe[0]);
		bsort(data, count);
		FILE* outfile;
		if(outfile = fopen(fname, "w")) {
			for(int i = 0; i < count; i++) {
				fprintf(outfile, "%s\n", data[i]);
				free(data[i]);
			}
			free(data);
			fclose(outfile);
			exit(0);
		} 
	}
	fprintf(stderr, "Error: fopen or fdopen returned NULL in childsort\n");
	exit(-1);
}

int* splitfile(char* fname, int fd1, int fd2) {
	FILE* file, *pipe1, *pipe2;
	if((file = fopen(fname, "r")) && (pipe1 = fdopen(fd1, "w")) && (pipe2 = fdopen(fd2, "w"))) {
		char line[LLEN];
		int ret;
		int count1 = 0;
		int count2 = 0;
		int flip = 0;
		while(ret != EOF) {
			for(int i = 0; i < ALEN; i++) {
				if((ret = fscanf(file, "%s", line)) == EOF) break;
				if(flip) {
					fprintf(pipe2, "%s\n", line);
					count2++;
				} else {
					fprintf(pipe1, "%s\n", line);
					count1++;
				}
			}
			flip = !flip;
		}
		fclose(file);
		fclose(pipe1);
		fclose(pipe2);
		close(fd1);
		close(fd2);
		int* retarr = (int*)malloc(sizeof(int)*2);
		retarr[0] = count1;
		retarr[1] = count2;
		return retarr;
	}	
	fprintf(stderr, "Error: fopen or fdopen returned NULL in splitfile\n");
	exit(-1);
}

char** readintoarr(FILE* in, int end) {
	int count = 0;
	int charsize = sizeof(char*);
	int currsize = charsize*ALEN;
	char** arr = (char**)malloc(currsize);
	char line[LLEN];	
	while(count < end && fscanf(in, "%s", line) != EOF) {
		if((count*charsize) > currsize) {
			currsize += charsize*ALEN;
			arr = (char**)realloc(arr, currsize);
		}
		arr[count] = strdup(line);
		count++;
	}
	return arr;
}

char** merge(char** arr1, char** arr2, int size1, int size2) {
	char** ret = (char**)malloc(sizeof(char*)*(size1+size2));
	int i = 0, count1 = 0, count2 = 0;
	while(count1 < size1 && count2 < size2) {
		if(strcmp(arr1[count1], arr2[count2]) < 0) {
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
	return;
}

void childmerge(int* pipe, char* outname, int count1, int count2) {
	close(pipe[1]);
	FILE* in, *out;
	if((in = fdopen(pipe[0], "r")) && (out = fopen(outname, "w"))) {
		char** arr1 = readintoarr(in, count1);
		char** arr2 = readintoarr(in, count2);
		fclose(in);
		close(pipe[0]);
		char** sorted = merge(arr1, arr2, count1, count2);
		freearr(arr1, count1);
		freearr(arr2, count2);
		int total = count1 + count2;
		for(int i = 0; i < total; i++) {
			fprintf(out, "%s\n", sorted[i]);
			free(sorted[i]);
		}
		free(sorted);
		fclose(out);
		exit(0);
	}
	fprintf(stderr, "Error: fopen or fdopen returned NULL in childmerge\n");
	exit(-1);
}

void copyclose(FILE* file, FILE* into) {
	char line[LLEN];
	while(fscanf(file, "%s", line) != EOF) {
		fprintf(into, "%s\n", line);
	}
	fclose(file);
	return;
}

void sendformerge(int fd, char* fname1, char* fname2) {
	FILE* pipe, *file1, *file2;
	if((pipe = fdopen(fd, "w")) && (file1 = fopen(fname1, "r")) && (file2 = fopen(fname2, "r"))) {
		copyclose(file1, pipe);
		copyclose(file2, pipe);		
		fclose(pipe);
		close(fd);
		return;
	}
	fprintf(stderr, "Error: fopen or fdopen returned NULL in sendformerge\n");
	exit(-1);
}

int main() {
	char* inname = "d1.dat";
	char* fname1 = "half1";
	char* fname2 = "half2";
	char* outname = "sorted";

	int* pipe1 = createpipe();
	int child1 = createchild();	
	if(!child1) {
		childsort(pipe1, fname1);
	}
	close(pipe1[0]);		
	
	int* pipe2 = createpipe();
	int child2 = createchild();
	if(!child2) {
		close(pipe1[1]);
		childsort(pipe2, fname2);
	}
	close(pipe2[0]);

	int* counts = splitfile(inname, pipe1[1], pipe2[1]);
	free(pipe1);
	free(pipe2);
	waitpid(child1, NULL, 0);
	waitpid(child2, NULL, 0);

	int* pipe3 = createpipe();
	int child3 = createchild();
	if(!child3) {
		childmerge(pipe3, outname, counts[0], counts[1]);
	}
	close(pipe3[0]);

	sendformerge(pipe3[1], fname1, fname2);	
	free(pipe3);
	waitpid(child3, NULL, 0);

	printf("\nDone! The %d elements in '%s' have been sorted and placed into '%s'.\n", (counts[0] + counts[1]), inname, outname);
	printf("The two sorted halves before merging can be viewed in '%s' and '%s'.\n\n", fname1, fname2);
	printf("'I told my friend if he ever needed help learning C, I could give him some pointers and he could put me down as a reference.'\n\n");

	free(counts);

	return 0;
}