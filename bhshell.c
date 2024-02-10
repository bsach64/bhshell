#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include "bhshell.h"

#define READLINE_BUFFER_SIZE 32
#define bhshell_TOK_BUFSIZE 64
#define bhshell_TOK_DELIM " \t\r\n\a"

char* bhshell_builtin_str[] = {
	"cd",
	"help",
	"exit",
};

int (*bhshell_builtin_func[]) (char**) = {
	&bhshell_cd,
	&bhshell_help,
	&bhshell_exit,
};

void bhshell_loop() {
	char* line; 
	char** args; 
	int status = 1;
	char* dir = NULL;

	do {
		dir = getcwd(NULL, 0);
		printf("[%s] $ ", dir);
		line = bhshell_read_line();
		args = bhshell_split_line(line);
		status = bhshell_execute(args); 
		
		free(dir);
		free(line);
		free(args);

	} while(status); 
}

char* bhshell_read_line() {
	size_t bufsize = READLINE_BUFFER_SIZE;
	size_t position = 0;
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
			char* newbuffer = realloc(buffer, bufsize);
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

int bhshell_execute(char** args) {
	if (args[0] == NULL) {
		return 1;
	}

	for (int i = 0; i < bhshell_num_builtins(); i++) {
		if (strcmp(args[0], bhshell_builtin_str[i]) == 0) {
			return (*bhshell_builtin_func[i])(args);
		}
	}
	return bhshell_launch(args);
}

int bhshell_launch(char** args) {
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			perror("bhshell");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		// error forking
		perror("bhshell");
	} else {
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

int bhshell_cd(char** args) {
	if (args[1] == NULL) {
		fprintf(stderr, "bhshell: expected argument to \"cd\" into\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("bhshell");
		}
	}
	return 1;
}

int bhshell_help(char** args) {
	printf("A simple shell to understand processes.\n");
	printf("The following functions are builtin:\n");

	int count = bhshell_num_builtins();
	for (int i = 0; i < count; i++) {
		printf("\t %d. %s\n", i + 1, bhshell_builtin_str[i]);
	}
	return 1;
}

int bhshell_exit(char** args) {
	return 0;
}

int bhshell_num_builtins() {
	return sizeof(bhshell_builtin_str) / sizeof(char*);
}
