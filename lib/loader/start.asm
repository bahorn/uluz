extern main

global _start

_start:
    call main
self:
    leave
    ret
