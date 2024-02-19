#include <stdlib.h>

#ifndef DYNAMICARR_H
#define DYNAMICARR_H

typedef struct str {
	char* string;
	size_t position;
	size_t bufsize;
} str;

str* new_str();
str* append_char(str* s, char c);
void destroy_str(str* s);
char* get_string(str* s);

#endif
