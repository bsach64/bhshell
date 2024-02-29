#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "dynamicarr.h"
#include "input.h"

void test_dynamicarr();
void test_parse();

int main(void) {
	printf("Running Tests for dynamic arrays...\n");
	test_dynamicarr();
	
	printf("Running Tests for parser...\n");
	test_parse();
	
	printf("All Tests ran successfully...\n");
}

void test_dynamicarr() {
	str s = { DA_NULL };

	da_append(&s, 'a');
	assert(s.items[0] == 'a');	
	da_append(&s, 'b');
	assert(s.items[1] == 'b');	
	da_append(&s, 'c');

	char* string = get_string(&s);
	assert(strlen(string) == 3);
	free(string);

	arg_list l = { DA_NULL };
	da_append(&l, "hello");
	da_append(&l, "world");
	assert(
		strcmp(l.items[0], "hello") == 0 &&
		strcmp(l.items[1], "world") == 0 &&
		l.position == 2
	);
	char** args = get_args(l);
	assert(
		strcmp(args[0], "hello") == 0 &&
		strcmp(args[1], "world") == 0
		&& args[2] == NULL
	);
	free(args);
	free(l.items);
}

void test_parse() {
	char* empty = "";
	command* cmd = bhshell_parse(empty);
	assert(cmd->args == NULL);
	assert(cmd->pipe_args == NULL);
	assert(cmd->redirect_file_name == NULL);
	// destroy_cmd(cmd);

	char* simple = "ls";
	cmd = bhshell_parse(simple);
	assert(cmd->redirect_file_name == NULL);
	assert(strcmp(cmd->args[0], "ls") == 0);
	assert(cmd->args[1] == NULL);
	// destroy_cmd(cmd);

	char* spaces = "  ";
	cmd = bhshell_parse(spaces);
	assert(cmd->redirect_file_name == NULL);
	assert(cmd->args[0] == NULL);
	// destroy_cmd(cmd);
	
	char* multiple = "ls -abc --aad --xx wow";
	cmd = bhshell_parse(multiple);
	assert(
		strcmp(cmd->args[0], "ls") == 0 &&
		strcmp(cmd->args[1], "-abc") == 0 &&
		strcmp(cmd->args[2], "--aad") == 0 &&
		cmd->args[5] == NULL	
	);

	char* redirect = "ls -abc   > wow.txt";
	cmd = bhshell_parse(redirect);
	assert(
		strcmp(cmd->args[0], "ls") == 0 &&
		strcmp(cmd->args[1], "-abc") == 0 &&
		cmd->args[2] == NULL &&
		strcmp(cmd->redirect_file_name, "wow.txt") == 0
	);

	char* pipes = "ls | wow";
	cmd = bhshell_parse(pipes);
	assert(
		strcmp(cmd->args[0], "ls") == 0 &&
		cmd->args[1] == NULL &&
		strcmp(cmd->pipe_args[0], "wow") == 0 &&
		cmd->pipe_args[1] == NULL &&
		cmd->redirect_file_name == NULL
	);

	char* invalid_redirect = "ls  >   ";
	cmd = bhshell_parse(invalid_redirect);
	assert(cmd == NULL);
	
	char* invalid_pipe = "ls  |   ";
	cmd = bhshell_parse(invalid_pipe);
	assert(cmd == NULL);

	char* newline = "\n";
	cmd = bhshell_parse(newline);
	assert(cmd == NULL);
}
