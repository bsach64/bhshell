#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "input.h"

#define READLINE_BUFFER_SIZE 32
#define bhshell_TOK_BUFSIZE 64
#define bhshell_TOK_DELIM " \t\r\n\a"
#define FILE_NAME_BUFSIZE 32


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

char** bhshell_split_line(char* line) {
	int bufsize = bhshell_TOK_BUFSIZE, position = 0;
	char** tokens = malloc(bufsize * sizeof(char*));
	char* token;

	if (!tokens) {
		fprintf(stderr,"bhshell: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, bhshell_TOK_DELIM);

	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += bhshell_TOK_BUFSIZE;
			char** new_tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!new_tokens) {
				fprintf(stderr, "bhshell: allocation error\n");
				free(tokens);
				exit(EXIT_FAILURE);
			}
			tokens = new_tokens;
		}

		token = strtok(NULL, bhshell_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

command* bhshell_parse(char* line) {
	command* cmd = malloc(sizeof(command));
	size_t bufsize = bhshell_TOK_BUFSIZE;
	size_t position = 0;
	size_t line_length = strlen(line);

	// allocates space for pointers to each of the strings
	char** tokens = malloc(bufsize * sizeof(char*));
	
	if (!tokens) {
		fprintf(stderr, "bhshell: allocation error\n");
		exit(EXIT_FAILURE);
	}

	char* current_token = malloc(sizeof(char) * bhshell_TOK_BUFSIZE);

	if (!current_token) {
		fprintf(stderr, "bhshell: allocation error\n");
		free(tokens);
		exit(EXIT_FAILURE);
	}

	size_t cur_token_pos = 0;
	size_t cur_token_buf = bhshell_TOK_BUFSIZE;
	for (size_t i = 0; i < line_length; i++) {
		if (line[i] == ' ' || line[i] == '\n' || line[i] == '\t' || line[i] == '\r') {
			current_token = append_char(current_token, &cur_token_buf, &cur_token_pos, '\0');
			tokens = append_token(tokens, &bufsize, &position, current_token);
			cur_token_pos = 0; cur_token_buf = bhshell_TOK_BUFSIZE;
			current_token = malloc(sizeof(char) * bhshell_TOK_BUFSIZE);
			if (!current_token) {
				fprintf(stderr, "bhshell: allocation error\n");
				destroy_tokens(tokens, position);
				free(cmd);
				exit(EXIT_FAILURE);
			}

		} else if (line[i] == '>') {
			cmd->does_redirect = true;
			if (cur_token_pos > 0) {
				tokens = append_token(tokens, &bufsize, &position, current_token);
			} else {
				free(current_token);
			}
			tokens = append_token(tokens, &bufsize, &position, NULL);
			cmd->args = tokens;

			char* file_name = malloc(sizeof(char) * FILE_NAME_BUFSIZE);
			size_t fn_buf = FILE_NAME_BUFSIZE;
			size_t fn_pos = 0;

			if (!file_name) {
				fprintf(stderr, "bhshell: allocation error\n");
				destroy_tokens(tokens, position);
				free(cmd);
				exit(EXIT_FAILURE);
			}
			for (size_t j = i + 1; j < line_length; j++) {
				if (fn_pos == 0 && (line[j] == ' ' || line[j] == '\n' || line[j] == '\r' || line[j] == '\t')) {
					continue;
				}
				file_name = append_char(file_name, &fn_buf, &fn_pos, line[j]);
			}
			cmd->redirect_file_name = file_name;
			break;

		} else {
			current_token = append_char(current_token, &cur_token_buf, &cur_token_pos, line[i]);
		}
	}

	return cmd;
}

char** append_token(char** tokens, size_t* bufsize, size_t* position,  char* token) {
	if (!tokens) {
		fprintf(stderr, "bhshell: allocation error");
		exit(EXIT_FAILURE);
	}

	tokens[*position] = token;
	(*position)++;
	if ((*position) >= (*bufsize)) {
		(*bufsize) += bhshell_TOK_BUFSIZE;
		char** new_tokens = realloc(tokens, (*bufsize) * sizeof(char*));
		if (!new_tokens) {
			fprintf(stderr, "bhshell: allocation error\n");
			free(tokens);
			exit(EXIT_FAILURE);
		}
		tokens = new_tokens;
	}
	return tokens;
}

char* append_char(char* token, size_t* bufsize, size_t* position, char character) {
	if (!token) {
		fprintf(stderr, "bhshell: allocation error");
		exit(EXIT_FAILURE);
	}

	token[*position] = character;
	(*position)++;
	if ((*position) >= (*bufsize)) {
		(*bufsize) += bhshell_TOK_BUFSIZE;
		char* new_token = realloc(token, (*bufsize) * sizeof(char));
		if (!new_token) {
			fprintf(stderr, "bhshell: allocation error\n");
			free(token);
			exit(EXIT_FAILURE);
		}
		token = new_token;
	}
	return token;
}


void destroy_tokens(char** tokens, size_t position) {
	if (position < 1) {
		return;
	}
	for (size_t i = 0; i < position; i++) {
		free(tokens[i]);
	}
	free(tokens);
}

void destroy_cmd(command* cmd) {
	if (!cmd) return;

	size_t i = 0;
	while(cmd->args[i] != NULL) {
		free(cmd->args[i]);
		i++;
	}
	free(cmd);
}

