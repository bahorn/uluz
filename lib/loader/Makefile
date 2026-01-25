build:
	make -C cleanup
	make -C payload
	nasm -f bin loader.asm -o loader.bin
	xxd -i loader.bin > loader.h
