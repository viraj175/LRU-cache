#ifndef DEBUG_H
#define DEBUG_H

// Defined color codes to help debug.
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

#include <stdio.h>


#ifdef DEBUG
    // For logging error message
    #define LOG_ERROR_MSG(msg) do { \
        fprintf(stderr, BLUE "[INFO]" RESET " %s, %s at %s:%d\n", \
                (msg), __func__, __FILE__, __LINE__); \
    } while(0)
#else
    #define LOG_ERROR_MSG(msg) do { } while(0)
#endif


// safely free the memory allocated
#define SAFE_FREE(ptr) do { \
    if (ptr) { \
        free(ptr); \
        (ptr) = NULL; \
    } \
} while(0)

#endif // DEBUG_H
