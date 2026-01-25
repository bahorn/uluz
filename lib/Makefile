build:
	make -C loader
	gcc -c -Wall -Werror -fpic main.c
	gcc -shared -nostartfiles -o libfoo.so main.o
