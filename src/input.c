#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "input.h"
#include "dynamicarr.h"
#include "xalloc.h"

char* bhshell_read_line() {
	str s = { DA_NULL };
	while(1) {
		int c = getchar();
		if (c == '\n' || c == EOF) {
			char* string = get_string(&s);
			return string;
		} else {
			da_append(&s, c);
		}
	}
}

command* bhshell_parse(char* line) {
	command* cmd = new_command();
	size_t line_length = strlen(line);
	
	if (line_length == 0) {
		return cmd;
	}

	arg_list args = { DA_NULL };
	str s = { DA_NULL };	

	for (size_t i = 0; i < line_length; i++) {
		if (line[i] == ' ' || line[i] == '\n' || line[i] == '\t' || line[i] == '\r') {
			if (s.position > 0) {
				char* string = get_string(&s);
				da_append(&args, string);
				s.bufsize = 0; s.position = 0; s.items = NULL;
			}
			continue;
		} else if (line[i] == '>') {
			if (s.position > 0) {
				char* string = get_string(&s);
				printf("%s\n", string);
				da_append(&args, string);
			} else {
				free(s.items);
			}
			
			da_append(&args, NULL);

			s.bufsize = 0; s.position = 0; s.items = NULL;
			
			if (i + 1 >= line_length) {
				destroy_cmd(cmd);
				free(s.items);
				return NULL;
			}
			
			for (size_t j = i + 1; j < line_length; j++) {
				if (s.position == 0 && (line[j] == ' ' || line[j] == '\n' || line[j] == '\t' || line[j] == '\r')) {
					continue;
				}
				da_append(&s, line[j]);
			}
			
			if (s.position == 0) {
				destroy_cmd(cmd);
				return NULL;
			}

			char* string = get_string(&s);

			cmd->redirect_file_name = string;
			cmd->args = args.items;
			return cmd;
		} else if (line[i] == '|') {
			arg_list pipe_args = { DA_NULL };
			if (s.position > 0) {
				char* string = get_string(&s);
				da_append(&args, string);
			} else {
				free(s.items);
				s.bufsize = 0; s.position = 0; s.items = NULL;
			}
			
			da_append(&args, NULL);

			free(s.items);
			s.bufsize = 0; s.position = 0; s.items = NULL;
			if (i + 1 >= line_length) {
				destroy_cmd(cmd);
				free(s.items);
				return NULL;
			}

			for (size_t j = i + 1; j < line_length; j++) {
				if (s.position == 0 && (line[j] == ' ' || line[j] == '\n' || line[j] == '\t' || line[j] == '\r')) {
					continue;
				} else if (line[j] == ' ' || line[j] == '\n' || line[j] == '\t' || line[j] == '\r') {
					char* string = get_string(&s);
					da_append(&pipe_args, string);
					free(s.items);
					s.bufsize = 0; s.position = 0; s.items = NULL;
				} else {
					da_append(&s, line[j]);
				}
			}
			if (s.position > 0) {
				char* string = get_string(&s);
				da_append(&pipe_args, string);
			} else {
				destroy_cmd(cmd);
				return NULL;
			}

			da_append(&pipe_args, NULL);
			cmd->args = args.items;
			cmd->pipe_args = pipe_args.items;
			return cmd;
		} else {
			da_append(&s, line[i]);
		}
	}

	if (cmd->redirect_file_name == NULL && cmd->pipe_args == NULL) {
		if (s.position > 0) {
			char* string = get_string(&s);
			da_append(&args, string);
		}
		da_append(&args, NULL);
		cmd->args = args.items;
	}
	return cmd;
}

command* new_command() {
	command* cmd = xmalloc(sizeof(command));
	
	cmd->args = NULL;
	cmd->pipe_args = NULL;
	cmd->redirect_file_name = NULL;
	return cmd;
}

void destroy_cmd(command* cmd) {
	destroy_args(cmd->args);
	destroy_args(cmd->pipe_args);
	free(cmd);
}
