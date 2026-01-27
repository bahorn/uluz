#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <sys/mman.h>


#define EGG_NAME "/memfd:egg"

int is_delimiter(char c, const char* delim)
{
    while (*delim) {
        if (c == *delim) {
            return 1;
        }
        delim++;
    }
    return 0;
}

char *strtok(char* str, const char* delim)
{
    static char* saved_ptr = NULL;
    char* token_start;

    // Use provided string or continue from last position
    if (str) {
        saved_ptr = str;
    }

    if (!saved_ptr || *saved_ptr == 0) {
        return NULL;
    }

    // Skip leading delimiters
    while (*saved_ptr && is_delimiter(*saved_ptr, delim)) {
        saved_ptr++;
    }

    if (*saved_ptr == 0) {
        return NULL;
    }

    // Mark token start
    token_start = saved_ptr;

    // Find token end
    while (*saved_ptr && !is_delimiter(*saved_ptr, delim)) {
        saved_ptr++;
    }

    // Null-terminate token if not at end of string
    if (*saved_ptr) {
        *saved_ptr = '\0';
        saved_ptr++;
    }

    return token_start;
}


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
    alarm(10);
    return 0;
}


int main()
{
    remove_mappings();
    payload();
}
