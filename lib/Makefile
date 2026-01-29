build:
	make -C ../payload
	gcc -c -Wall -Werror -fpic main.c
	gcc -shared -nostartfiles -o libfoo.so main.o
