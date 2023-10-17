#include "smile.h"

int mgetc(MILE* stream) {
	char* c = (char*)malloc(sizeof(char*));
	if(!mread((void*)c, sizeof(char), stream)) {
		PRINTERR("Error: mread returned 0\n");
		return -1;
	}
	int ret = *c;
	free(c);
	return ret;
}

int mputc(int c, MILE* stream) {
	int* ch = &c;
	if(!mwrite(ch, sizeof(char), stream)) {
		PRINTERR("Error: mwrite returned 0\n");
		return -1;
	}	
	return c;
}

int mread_int(int* a, MILE* stream) {
	if(!mread(a, sizeof(int), stream)) {
		PRINTERR("Error: mread returned 0\n");
		return -1;
	}
	return 0;
}

int mwrite_int(int i, MILE* stream) {
	int* ptr = &i;
	if(!mwrite(ptr, sizeof(int), stream)) {
		PRINTERR("Error: mwrite returned 0\n");
		return -1;
	}
	return 0;
}