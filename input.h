#include <stdlib.h>

#ifndef INPUT_H
#define INPUT_H

typedef struct {
	char** args;
	bool does_redirect;
	char* redirect_file_name;
} command;

char* bhshell_read_line();
char** bhshell_split_line(char* line);
command* bhshell_parse(char* line);
char** append_token(char** tokens, size_t* bufsize, size_t* position, char* token);
char* append_char(char* token, size_t* bufsize,  size_t* position, char character);
void destroy_tokens(char** tokens, size_t position);
void destroy_cmd(command* cmd);


#endif
