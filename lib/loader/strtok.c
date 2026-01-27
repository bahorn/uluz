// LLM generated, just to save effort writing this

#define NULL 0

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

