#ifndef BHSHELL_H
#define BHSHELL_H

void bhshell_loop();
int bhshell_execute(char** args);
int bhshell_launch(char** args); 
int bhshell_cd(char** args);
int bhshell_help(char** args);
int bhshell_exit(char** args);
int bhshell_num_builtins();

#endif // !BHSHELL_H
