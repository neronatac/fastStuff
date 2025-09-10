#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "utils.h"

const char *const usages[] = {
    "fastAES [cmd] [args]",
    "fastAES [cmd] -h (for help)",
    NULL,
};

int parse_long_with_suffix(const char* data, uint64_t *value) {
    if (data == NULL || value == NULL) {
        return -1;
    }
    char *endptr;
    long val = strtol(data, &endptr, 10);
    if (endptr == data) {
        return -1; // No digits were found
    }
    switch (*endptr) {
        case 'k':
        case 'K':
            val *= 1024;
            endptr++;
            break;
        case 'M':
        case 'm':
            val *= 1024 * 1024;
            endptr++;
            break;
        case 'G':
        case 'g':
            val *= 1024 * 1024 * 1024;
            endptr++;
            break;
        case '\0':
            break; // No suffix
        default:
            return -1; // Invalid suffix
    }
    if (*endptr != '\0' || val <= 0) {
        return -1; // Extra characters after number or out of range
    }
    *value = (uint64_t)val;
    return 0;
}
