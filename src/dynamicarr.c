#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamicarr.h"

#define STR_BUFSIZE 32

str* new_str() {
	str* s = malloc(sizeof(str));
	if (!s) {
		fprintf(stderr, "bhshell: allocation error\n");
		return NULL;
	}
	s->bufsize = STR_BUFSIZE;
	s->string = malloc(sizeof(char) * STR_BUFSIZE);

	if (!s->string) {
		free(s);
		fprintf(stderr, "bhshell: allocation error\n");
		return NULL;
	}
	s->position = 0;
	return s;
}

str* append_char(str* s, char c) {

	if (!s) return NULL;

	s->string[s->position] = c;
	(s->position)++;
	
	if (s->position >= s->bufsize) {
		(s->bufsize) += STR_BUFSIZE;
		char* new_string = realloc(s->string, sizeof(char) * (s->bufsize));
		if (!new_string) {
			fprintf(stderr, "bhshell: allocation error\n");
			destroy_str(s);
			return NULL;
		}
		s->string = new_string;
	}
	return s;
}

void destroy_str(str* s) {
	free(s->string);
	free(s);
}

char* get_string(str* s) {
	s = append_char(s, '\0');
	
	if (!s) return NULL;

	char* string = malloc(sizeof(char) * (s->position));

	if (!string) {
		destroy_str(s);
		return NULL;
	}

	memcpy(string, s->string, sizeof(char) * s->position);
	destroy_str(s);
	return string;
}
