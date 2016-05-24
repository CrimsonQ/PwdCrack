#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "options.h"

typedef struct
{
    FILE * file;
    long int bound;

} dictionaryThreadArg;

void * dictionary_thread(void * arg);

unsigned long getFileSize(FILE * file);

void determineBounds(FILE * fileArr[], int i, long int * partBounds, int ncores, unsigned long partFile);

#endif // DICTIONARY_H
