BITS 64

_start:
    call cleanup
    call payload
    leave
    ret

cleanup:
    incbin "cleanup/cleanup.bin"

payload:
    incbin "payload/payload.bin"
