#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#include "bhshell.h"
#include "input.h"

int main(void) {
	char* line = bhshell_read_line();
	printf("line: %s\n", line);
	
	command* cmd = bhshell_parse(line);

	size_t i = 0;
	while (cmd->args[i] != NULL) {
		printf("[%s]", cmd->args[i]);
		i++;
	}

	printf("\nredirect: %s\n", cmd->redirect_file_name);
	
	free(line);
	destroy_cmd(cmd);
	// char** args = bhshell_split_line(line);
	// char* output_file = get_output_file(args);
	// 
	// if (!output_file) {
	// 	fprintf(stderr, "bhshell: please enter a valid file name\n");
	// 	exit(EXIT_FAILURE);
	// }
	//
	// printf("output_file: %s\n", output_file);
	//
	// int file = open(output_file, O_WRONLY | O_CREAT, 0777);
	//
	// dup2(file, STDOUT_FILENO);
	//
	// close(file);
	// free(line);
	// free(args);
	// free(output_file);
	
	// bhshell_loop();
	return EXIT_SUCCESS;
}
