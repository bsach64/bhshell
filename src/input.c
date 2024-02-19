#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "input.h"
#include "dynamicarr.h"

#define ARG_LIST_BUFFER_SIZE 10

#define FREE_ON_ERR(F) if (!F) {\
	destroy_cmd(cmd);\
	destroy_args(args);\
	return NULL;\
}


char* bhshell_read_line() {
	str* s = new_str();
	if (!s) return NULL;
	while(1) {
		int c = getchar();
		if (c == '\n' || c == EOF) {
			char* string = get_string(s);
			if (!string) return NULL;
			
			return string;
		} else {
			s = append_char(s, c);
			if (!s) return NULL;
		}
	}
}

command* bhshell_parse(char* line) {
	command* cmd = new_command();
	if (!cmd) return NULL;

	size_t line_length = strlen(line);

	char** args = malloc(ARG_LIST_BUFFER_SIZE * sizeof(char*));
	size_t args_postion = 0, args_bufsize = ARG_LIST_BUFFER_SIZE;

	if (!args) {
		destroy_cmd(cmd);
		return NULL;
	}
	
	str* s = new_str();

	if (!s) {
		destroy_cmd(cmd);
		return NULL;
	}

	for (size_t i = 0; i < line_length; i++) {
		if (line[i] == ' ' || line[i] == '\n' || line[i] == '\t' || line[i] == '\r') {
			if (s->position > 0) {
				char* string = get_string(s);
		
				FREE_ON_ERR(string);

				args = append_arg(args, &args_postion, &args_bufsize, string);
				
				if (!args) {
					destroy_cmd(cmd);
					return NULL;
				}
				
				s = new_str();

				FREE_ON_ERR(s);
			}
			continue;
			
		} else if (line[i] == '>') {
			if (s->position > 0) {
				char* string = get_string(s);
				printf("%s\n", string);
				FREE_ON_ERR(string);

				args = append_arg(args, &args_postion, &args_bufsize, string);

				if (!args) {
					destroy_cmd(cmd);
					return NULL;
				}

			} else {
				destroy_str(s);
			}

			args = append_arg(args, &args_postion, &args_bufsize, NULL);

			if (!args) {
				destroy_cmd(cmd);
				return NULL;
			}

			s = new_str();
			
			FREE_ON_ERR(s);
				
			if (i + 1 >= line_length) {
				destroy_cmd(cmd);
				destroy_str(s);
				return NULL;
			}
			
			for (size_t j = i + 1; j < line_length; j++) {
				if (s->position == 0 && (line[j] == ' ' || line[j] == '\n' || line[j] == '\t' || line[j] == '\r')) {
					continue;
				}
				s = append_char(s, line[j]);

				FREE_ON_ERR(s);
			}
			
			if (s->position == 0) {
				destroy_cmd(cmd);
				destroy_str(s);
				return NULL;
			}

			char* string = get_string(s);

			FREE_ON_ERR(string);

			cmd->redirect_file_name = string;
			break;
		} else {
			s = append_char(s, line[i]);
			
			FREE_ON_ERR(s);
		}
	}

	if (cmd->redirect_file_name == NULL) {
		if (s->position > 0) {
			char* string = get_string(s);
			FREE_ON_ERR(string);

			args = append_arg(args, &args_postion, &args_bufsize, string);

			if (!args) {
				destroy_cmd(cmd);
				return NULL;
			}
		}
		args = append_arg(args, &args_postion, &args_bufsize, NULL);
		if (!args) {
			destroy_cmd(cmd);
			return NULL;
		}
	}

	cmd->args = args;
	return cmd;
}

char** append_arg(char** args, size_t* args_position, size_t* args_bufsize, char* arg) {
	if (!args) {
		return NULL;
	}

	args[*args_position] = arg;
	(*args_position)++;
	
	if (*args_position >= *args_bufsize) {
		(*args_bufsize) += ARG_LIST_BUFFER_SIZE;
		char** new_args = realloc(args, sizeof(char*) * (*args_bufsize));
		if (!new_args) {
			fprintf(stderr, "bhshell: allocation error\n");
			free(args);
			return NULL;
		}
		args = new_args;
	}
	return args;
}


command* new_command() {
	command* cmd = malloc(sizeof(command));
	
	if (!cmd) {
		fprintf(stderr, "bhshell: allocation error\n");
		return NULL;
	}
	cmd->args = NULL;
	cmd->redirect_file_name = NULL;
	return cmd;
}

void destroy_cmd(command* cmd) {
	destroy_args(cmd->args);
	free(cmd);
}

void destroy_args(char** args) {
	size_t i = 0;
	while (args[i] != NULL) {
		free(args[i]);
		i++;
	}
	free(args);
}
