#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#include "bhshell.h"
#include "input.h"

int main(void) {
	bhshell_loop();
	//
	// char* line = bhshell_read_line();
	//
	// command* cmd = bhshell_parse(line);
	//
	// if (cmd == NULL) {
	// 	printf("wow\n");
	// 	exit(EXIT_FAILURE);
	// }
	//
	// size_t i = 0; 
	// while (cmd->args[i] != NULL) {
	// 	printf("%s\n", cmd->args[i]);
	// 	i++;
	// }
	//
	// if (cmd->redirect_file_name == NULL) {
	// 	printf("wow\n");
	// }
	return EXIT_SUCCESS;
}
