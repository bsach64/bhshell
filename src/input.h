#include <stdlib.h>

#ifndef INPUT_H
#define INPUT_H

typedef struct {
	char** args;
	char** pipe_args;
	char* redirect_file_name;
} command;

char* bhshell_read_line();
command* bhshell_parse(char* line);

char** append_arg(char** args, size_t* args_position, size_t* args_bufsize, char* arg);

void destroy_args(char** args);
void destroy_cmd(command* cmd);
command* new_command();
#endif
