#ifndef MILE_H
#define MILE_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SIZE 100
#define FLAGS(str) (strcmp(str, "r") == 0 ? (O_RDONLY) : strcmp(str, "w") == 0 ? (O_WRONLY | O_TRUNC | O_CREAT) : strcmp(str, "a") == 0 ? (O_WRONLY | O_APPEND | O_CREAT) : -1)
#define PRINTERR(str) write(1, str, sizeof(str))

typedef struct MILE {	
	int _fd;
	char _mode;
	void* _buf;
	char _pos;
	char _end;
} MILE;

MILE* mopen(const char* name, char* mode);

void _readIn(MILE* m);
int _copyOut(MILE* m, void* into, int howmuch);
int mread(void* b, int len, MILE* m);

void _writeOut(MILE* m);
int _copyIn(MILE* m, void* from, int howmuch);
int mwrite(void* b, int len, MILE* m);

int mclose(MILE* m); 

#endif