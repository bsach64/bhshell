
bhshell: src/bhshell.c src/dynamicarr.c src/input.c src/main.c src/xalloc.c
	cc src/bhshell.c src/dynamicarr.c src/input.c src/main.c src/xalloc.c -o bhshell -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter

test: src/bhshell.c src/dynamicarr.c src/input.c src/test.c src/xalloc.c
	cc src/dynamicarr.c src/input.c src/test.c src/xalloc.c -o test -Wall -Wextra

clean:
	rm bhshell test
