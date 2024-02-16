#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "input.h"

#define READLINE_BUFFER_SIZE 32
#define ARG_LIST_BUFFER_SIZE 10
#define ARG_BUFFER_SIZE 32

char* bhshell_read_line() {
	size_t bufsize = READLINE_BUFFER_SIZE; size_t position = 0;
	char* buffer = malloc(bufsize * (sizeof(char)));
	int c;

	if (!buffer) {
		fprintf(stderr, "bhshell: allocation error");
		exit(EXIT_FAILURE);
	}

	while(1) {
		c = getchar();
		if (c == '\n' || c == EOF) { 
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}
		
		if (position + 1 >= bufsize) {
			bufsize += READLINE_BUFFER_SIZE;
			char* newbuffer = realloc(buffer, bufsize * sizeof(char));
			if (!newbuffer) {
				fprintf(stderr, "bhshell: allocation error\n");
				free(buffer); 
				exit(EXIT_FAILURE);
			}
			buffer = newbuffer;
		}
		position++;	
	}
}

command* bhshell_parse(char* line) {
	command* cmd = new_command();
	if (!cmd) {
		return NULL;
	}
	size_t line_length = strlen(line);

	char** args = malloc(ARG_LIST_BUFFER_SIZE * sizeof(char*));
	size_t args_postion = 0, args_bufsize = ARG_LIST_BUFFER_SIZE;

	if (!args) {
		fprintf(stderr, "bhshell: allocation error\n");
		free(cmd);
		return NULL;
	}
	
	char* new_arg = malloc(ARG_BUFFER_SIZE * sizeof(char));

	if (!new_arg) {
		fprintf(stderr, "bhshell: allocation error\n");
		free(args);
		free(cmd);
		return NULL;
	}
	
	size_t new_arg_position = 0, new_arg_bufsize = ARG_BUFFER_SIZE;

	for (size_t i = 0; i < line_length; i++) {
		if (line[i] == ' ' || line[i] == '\n' || line[i] == '\t' || line[i] == '\r') {
			if (new_arg_position > 0) {
				new_arg = append_char(new_arg, &new_arg_position, &new_arg_bufsize, '\0');
				
				if (!new_arg) {
					free(args);
					free(cmd);
					return NULL;
				}

				args = append_arg(args, &args_postion, &args_bufsize, new_arg);
				
				if (!args) return NULL;

				new_arg = malloc(ARG_BUFFER_SIZE * sizeof(char));
				
				if (!new_arg) {
					free(args);
					free(cmd);
					return NULL;
				}

				new_arg_position = 0; new_arg_bufsize = ARG_BUFFER_SIZE;
			} else {
				continue;
			}
		} else if (line[i] == '>') {
			if (new_arg_position > 0) {
				new_arg = append_char(new_arg, &new_arg_position, &new_arg_bufsize, '\0');
				
				if (!new_arg) {
					free(args);
					free(cmd);
					return NULL;
				}
				args = append_arg(args, &args_postion, &args_bufsize, new_arg);
				if (!args) return NULL;
			} else {
				free(new_arg);
			}

			args = append_arg(args, &args_postion, &args_bufsize, NULL);
			if (!args) return NULL;

			char* file_name = malloc(ARG_BUFFER_SIZE * sizeof(char));
			
			if (!file_name) {
				free(args);
				free(cmd);
				return NULL;
			}

			size_t file_name_position = 0, file_name_bufsize = ARG_BUFFER_SIZE;

			for (size_t j = i + 1; j < line_length; j++) {
				if (file_name_position == 0 && (line[j] == ' ' || line[j] == '\n' || line[j] == '\t' || line[j] == '\r')) {
					continue;
				}
				file_name = append_char(file_name, &file_name_position, &file_name_bufsize, line[j]);
				if (!file_name) {
					free(args);
					free(cmd);
					return NULL;
				}
			}
			file_name = append_char(file_name, &file_name_position, &file_name_bufsize, '\0');
			if (!file_name) {
				free(args);
				free(cmd);
				return NULL;
			}
			cmd->redirect_file_name = file_name;
			break;
		} else {
			new_arg = append_char(new_arg, &new_arg_position, &new_arg_bufsize, line[i]);
			
			if (!new_arg) {
				free(args);
				free(cmd);
				return NULL;
			}
		}
	}

	if (cmd->redirect_file_name == NULL) {
		args = append_arg(args, &args_postion, &args_bufsize, NULL);
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

char* append_char(char* arg, size_t* new_arg_position, size_t* new_arg_bufsize, char character) {
	if (!arg) return NULL;

	arg[*new_arg_position] = character;
	(*new_arg_position)++;
	
	if (*new_arg_position >= *new_arg_bufsize) {
		(*new_arg_bufsize) += ARG_BUFFER_SIZE;
		char* new_arg = realloc(arg, sizeof(char) * (*new_arg_bufsize));
		if (!new_arg) {
			fprintf(stderr, "bhshell: allocation error\n");
			free(arg);
			return NULL;
		}
		arg = new_arg;
	}
	return arg;
}

void destroy_cmd(command* cmd) {
	size_t i = 0;
	while (cmd->args[i] != NULL) {
		free(cmd->args[i]);
		i++;
	}
	free(cmd);
}
