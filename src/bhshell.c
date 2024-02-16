#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>

#include "dynamicarr.h"
#include "input.h"
#include "bhshell.h"

#define BUF_SIZE 64

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
		status = bhshell_execute(cmd); 
		free(dir);
		free(line);
		destroy_cmd(cmd);

	} while(status); 
}

int bhshell_execute(command* cmd) {
	if (cmd->args[0] == NULL) {
		return 1;
	}

	for (int i = 0; i < bhshell_num_builtins(); i++) {
		if (strcmp(cmd->args[0], bhshell_builtin_str[i]) == 0) {
			return (*bhshell_builtin_func[i])(cmd->args);
		}
	}
	return bhshell_launch(cmd);
}

int bhshell_launch(command* cmd) {
	pid_t pid, wpid;
	int status;

	int redirect_fd[2];
	if (cmd->redirect_file_name != NULL) {
		if (pipe(redirect_fd) == -1) {
			exit(EXIT_FAILURE);
		}
	}

	pid = fork();
	if (pid == 0) {
		if (cmd->redirect_file_name != NULL) {
			close(redirect_fd[0]);
			if (dup2(redirect_fd[1], STDOUT_FILENO) == -1) {
				fprintf(stderr, "bhshell: Could not redirect stdout to file\n");
				exit(EXIT_FAILURE);
			}
			close(redirect_fd[1]);
		}
		

		if (execvp(cmd->args[0], cmd->args) == -1) {
			perror("bhshell");
		}
		// execvp takes over the entire process
		// so if return backs to this process it has errored
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		// error forking
		perror("bhshell: Could not create child process");
	} else {
		if (cmd->redirect_file_name != NULL) {
			
			str* s = new_str();

			if (!s) {
				close(redirect_fd[0]);
				close(redirect_fd[1]);
				exit(EXIT_FAILURE);
			}
			char temp;
			
			close(redirect_fd[1]);
			int finished = read(redirect_fd[0], &temp, sizeof(char));
			if (finished == -1) {
				close(redirect_fd[0]);
				close(redirect_fd[1]);
				destroy_str(s);
				exit(EXIT_FAILURE);
			}
			while (finished != 0) {
				s = append_char(s, temp);

				if (!s) {
					close(redirect_fd[0]);
					close(redirect_fd[1]);
					exit(EXIT_FAILURE);
				}
				finished = read(redirect_fd[0], &temp, sizeof(char));
			}
			char* string = get_string(s);
			
			if (!string) {
				close(redirect_fd[0]);
				close(redirect_fd[1]);
				exit(EXIT_FAILURE);
			}
			
			close(redirect_fd[0]);

			FILE* f = fopen(cmd->redirect_file_name, "w");
			if (!f) {
				fprintf(stderr, "Could not open file\n");
				destroy_str(s);
				exit(EXIT_FAILURE);
			}
			size_t written = fwrite(string, strlen(string), 1, f);
			if (written == 0) {
				fprintf(stderr, "Could not write to file\n");
				free(string);
				exit(EXIT_FAILURE);
			}
			free(string); 
			fclose(f);
		}

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
	printf("A simple shell built to understand how processes work.\n");
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
