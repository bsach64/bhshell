#include <stdlib.h>

#ifndef INPUT_H
#define INPUT_H

typedef struct command {
	char** args;
	char** pipe_args;
	char* redirect_file_name;
} command;

char* bhshell_read_line();
command* bhshell_parse(char* line);

void destroy_cmd(command* cmd);
command* new_command();
#endif
