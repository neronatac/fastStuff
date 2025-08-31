#include <cpuid.h>
#include "aes.h"
#include "utils.h"

int is_aesni_supported() {
    int a, b, c, d;

    // check SSE / SSE2
    __cpuid(1, a, b, c, d);
    if(!((d >> 25) & 0b11)) {
        DEBUG_PRINT("SSE/SSE2 not supported\n");
        return 0;
    }

    // check AES-NI
    if(!((c >> 25) & 1)) {
        DEBUG_PRINT("aes-ni not supported\n");
        return 0;
    }

    DEBUG_PRINT("aes-ni supported!\n");
    return 1;
}