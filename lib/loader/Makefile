CFLAGS=-O0 \
	   -Wundef \
	   -Wno-trigraphs \
	   -fno-strict-aliasing \
	   -fno-common \
	   -fshort-wchar \
	   -Werror=return-type \
	   -Wno-format-security \
	   -mno-sse \
	   -mno-mmx \
	   -mno-sse2 \
	   -mno-3dnow \
	   -mno-avx \
	   -m64 \
	   -fno-plt \
	   -falign-jumps=1 \
	   -falign-loops=1 \
	   -mno-80387 \
	   -mno-fp-ret-in-387 \
	   -mskip-rax-setup \
	   -mtune=generic \
	   -mno-red-zone \
	   -Wno-sign-compare \
	   -fno-asynchronous-unwind-tables \
	   -fno-jump-tables \
	   -mharden-sls=all \
	   -fno-delete-null-pointer-checks \
	   -Wno-frame-address \
	   -Wno-format-truncation \
	   -Wno-format-overflow \
	   -Wno-address-of-packed-member \
	   -fno-allow-store-data-races \
	   -fno-stack-protector \
	   -Wimplicit-fallthrough=5 \
	   -Wno-main \
	   -Wno-unused-but-set-variable \
	   -Wno-unused-const-variable \
	   -fno-omit-frame-pointer \
	   -fno-optimize-sibling-calls \
	   -fno-stack-clash-protection \
	   -mrecord-mcount \
	   -mfentry \
	   -Wdeclaration-after-statement \
	   -Wvla \
	   -Wno-pointer-sign \
	   -Wno-stringop-truncation \
	   -Wno-zero-length-bounds \
	   -Wno-array-bounds \
	   -Wno-stringop-overflow \
	   -Wno-restrict \
	   -Wno-maybe-uninitialized \
	   -Wno-alloc-size-larger-than \
	   -fno-strict-overflow \
	   -fno-stack-check \
	   -fconserve-stack \
	   -Werror=date-time \
	   -Werror=incompatible-pointer-types \
	   -Werror=designated-init \
	   -Wno-packed-not-aligned \
	   -D_NOLIBC_STACKPROTECTOR_H \
	   -lc \
	   -Wl,-T,./linker.lds -Wl,-z,noexecstack \
	   -fno-zero-initialized-in-bss \
	   -mstackrealign \
	   -mgeneral-regs-only
ASFLAGS=-64

CC=../../toolchains/musl-patched/build/bin/musl-gcc
LD=ld


build:
	nasm -f elf64 start.asm
	$(CC) $(CFLAGS) -static-pie payload.c start.o -o payload.bin
	xxd -i payload.bin > payload.h
