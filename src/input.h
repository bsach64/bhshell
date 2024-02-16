#include <stdlib.h>

#ifndef INPUT_H
#define INPUT_H

typedef struct {
	char** args;
	char* redirect_file_name;
} command;

char* bhshell_read_line();
command* bhshell_parse(char* line);

char** append_arg(char** args, size_t* args_position, size_t* args_bufsize, char* arg);
char* append_char(char* arg, size_t* new_arg_position, size_t* new_arg_bufsize, char character);

void destroy_cmd(command* cmd);
command* new_command();
#endif
