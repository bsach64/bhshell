# A Simple Shell
A shell written in C to help understand processes.

## Features
Has 3 built in commands => [cd, help, exit].\
Also supports redirection.\
So,`ls > file.txt` works.

## Build
`git clone https://github.com/bsach64/bhshell.git`\
`chmod +x build.sh`\
`./build.sh`\
`./bhshell`

## TODO
- [ ] Rewrite for better organisation and error handling
    - [x] Rewrite to use xmalloc
    - [ ] Add tests
- [x] prompt shows current working directory
- [ ] Understand how creating processes actually works
- [x] Remove `exit(EXIT_FAILURE)`
- [ ] Rewrite the Parser {Basically}
    - [x] Simulate the ">" operator of $BASH
    - [ ] Support multiple ">" {allowing for `ls > file1.txt > file2.txt`}
    - [ ] Simulate the "|" operator of $BASH
- [ ] Maybe
    - [ ] Escape characters etc {echo "wow" should print wow instead of "wow"}
    - [ ] Implement some level of globbing (Meaning *.txt means all .txt files in the current dir)
