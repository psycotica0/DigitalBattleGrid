#include "chunk_string.h"

int chunk_readline_count(FILE* stream, string_chunk* chunk, int read) {
	char temp;
	int i=0;

	for (i=0; i < CHUNK_SIZE; i++) {
		if (feof(stream)) {
			chunk->value[i]='\0';
			return read;
		}
		temp = fgetc(stream);
		if (temp == EOF || temp == '\0' || temp == '\n') {
			chunk->value[i]='\0';
			return read;
		}
		read++;
		chunk->value[i]=temp;
	}

	if (feof(stream)) {
		chunk->next = NULL;
		return read;
	}

	temp = fgetc(stream);
	if (temp == EOF || temp == '\0' || temp == '\n') {
		chunk->next = NULL;
		return read;
	}

	ungetc(temp, stream);
	chunk->next = malloc(sizeof(string_chunk));
	chunk->next->next = NULL;
	if (chunk->next == NULL) {
		return;
	}

	return chunk_readline_count(stream, chunk->next, read);
}

int chunk_readline(FILE* stream, string_chunk* chunk) {
	return chunk_readline_count(stream, chunk, 0);
}

void chunk_printline(FILE* stream, string_chunk* chunk) {
	int i=0;
	for (i=0; i < CHUNK_SIZE; i++) {
		if (chunk->value[i] == '\0') {
			return;
		}
		fputc(chunk->value[i], stream);
	}

	if (chunk->next != NULL) {
		chunk_printline(stream, chunk->next);
	}
}

