#ifndef BHSHELL_H
#define BHSHELL_H

#include "input.h"

void bhshell_loop();
int bhshell_execute(command* cmd);
int bhshell_launch(command* cmd); 
int bhshell_cd(char** args);
int bhshell_help(char** args);
int bhshell_exit(char** args);
int bhshell_num_builtins();

#endif // !BHSHELL_H
