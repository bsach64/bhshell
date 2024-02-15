#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "input.h"
#include "bhshell.h"


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
	char** args; 
	int status = 1;

	do {
		char* dir = getcwd(NULL, 0);
		printf("[%s] $ ", dir);
		char* line = bhshell_read_line();
		command* cmd = bhshell_parse(line);
		status = bhshell_execute(cmd->args); 
		
		if (cmd->does_redirect) {

		}

		free(dir);
		free(line);
		destroy_cmd(cmd);

	} while(status); 
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
