#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

// Macro definitions

#define PLAINS_BIN "_plains.bin"
#define KEYS_BIN "_keys.bin"
#define CIPHERS_REF_BIN "_ciphers_ref.bin"
#define CIPHERS_BIN "_ciphers.bin"

// Functions in utils.c

/**
 * Parses a string representing an integer with a suffix (k, M, G)
 * and returns the integer value.
 * Returns 0 on success, -1 on error.
 */
int parse_long_with_suffix(const char* data, uint64_t *value);

// MISC
extern const char *const usages[];

#endif  // UTILS_H