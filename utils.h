#ifndef UTILS_H
#define UTILS_H


// Macro definitions

#ifdef DEBUG
    #define DEBUG_PRINT(...) fprintf( stderr, __VA_ARGS__ )
#else
    #define DEBUG_PRINT(...) do{ } while ( 0 )
#endif

#define PLAINS_BIN "_plains.bin"
#define KEYS_BIN "_keys.bin"
#define CIPHERS_BIN "_ciphers.bin"
#define CIPHERS_BENCH_BIN "_ciphers_bench.bin"

// Functions in utils.c

/**
 * Prints the help message
 */
void help();

// MISC
extern const char *const usages[];

#endif  // UTILS_H