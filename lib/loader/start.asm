extern main
extern __init_libc
extern __libc_start_init

global _fake_start

_fake_start:
    ; rdi contains environ
    lea rsi, [rel progname]
    call __init_libc
    call __libc_start_init
    call main
    leave
    ret

progname:
    db "prog"
    db 0
