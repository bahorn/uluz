#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <sys/mman.h>


#define EGG_NAME "/memfd:egg"

void remove_mappings(void)
{
    FILE *file;
    char line[1024];
    void *start = NULL;
    void *start_curr = NULL;
    void *end = NULL;

    // Open the maps file
    file = fopen("/proc/self/maps", "r");
    if (file == NULL) return;

    while (fgets(line, sizeof(line), file)) {
        char *filename;
        char *token;
        char *address;
        int field = 0;

        // Skip first 5 fields to get to filename
        token = strtok(line, " \t");
        address = token;
        while (token != NULL && field < 5) {
            token = strtok(NULL, " \t");
            field++;
        }

        // If we have a 6th field, that's our filename
        if (token != NULL) {
            // Remove newline if present
            filename = strtok(token, "\n");
            if (!filename) {
                continue;
            }
            if (strcmp(filename, EGG_NAME) == 0) {
                // printf("%s\n", filename);
                // puts(address);
                // puts("egg found");
                token = strtok(address, "-");

                start_curr = (void *) strtol(token, NULL, 16);
                if (start == NULL) {
                    start = start_curr;
                }
                token = strtok(NULL, "-");
                end = (void *) strtol(token, NULL, 16);
                munmap(start, end - start);
            }
        }
    }

    fclose(file);
}

static void dothis(int signum) {
    write(1, "h", 1);
    alarm(5);
}


int payload()
{
    struct sigaction psa;
    void (*func_ptr)(int) = &dothis;
    psa.sa_handler = func_ptr;
    psa.sa_flags = 0;
    sigemptyset(&psa.sa_mask);

    sigaction(SIGALRM, &psa, NULL);
    alarm(1);
    return 0;
}


int main()
{
    remove_mappings();
    payload();
}
