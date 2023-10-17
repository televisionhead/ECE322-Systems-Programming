#ifndef SMILE_H
#define SMILE_H

#include "mile.h"

int mgetc(MILE* stream);
int mputc(int c, MILE* stream);

int mread_int(int* a, MILE* stream);
int mwrite_int(int i, MILE* stream);

#endif