#ifndef _CHUNK_STRING_H
#define _CHUNK_STRING_H

#include<stdio.h>
#include<stdlib.h>

#ifndef CHUNK_SIZE
#define CHUNK_SIZE 10
#endif

typedef struct string_chunk {
	char value[CHUNK_SIZE];
	struct string_chunk* next;
} string_chunk;


int chunk_readline_count(FILE*, string_chunk*, int);

int chunk_readline(FILE*, string_chunk*);

void chunk_printline(FILE*, string_chunk*);

#endif
