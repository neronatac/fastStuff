#include <cpuid.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"
#include "utils.h"

int is_aesni_supported() {
    int a, b, c, d;

    // check SSE / SSE2
    __cpuid(1, a, b, c, d);
    if(!((d >> 25) & 0b11)) {
        printf("SSE/SSE2 not supported\n");
        return 0;
    }

    // check AES-NI
    if(!((c >> 25) & 1)) {
        printf("aes-ni not supported\n");
        return 0;
    }

    printf("aes-ni supported!\n");
    return 1;
}

int aes_treat_files(const char *name) {
    // open plain and key files
    char* plains_fn = malloc(strlen(name) + sizeof(PLAINS_BIN) + 1);
    strcpy(plains_fn, name);
    strcat(plains_fn, PLAINS_BIN);
    FILE *plains_f = fopen(plains_fn, "rb");
    free(plains_fn);
    if (plains_f == NULL) {
        printf("Error while opening plains file\n");
        return -1;
    }

    char* keys_fn = malloc(strlen(name) + sizeof(KEYS_BIN) + 1);
    strcpy(keys_fn, name);
    strcat(keys_fn, KEYS_BIN);
    FILE *keys_f = fopen(keys_fn, "rb");
    free(keys_fn);
    if (keys_f == NULL) {
        printf("Error while opening keys file\n");
        return -1;
    }

    // create result file (ciphers_bench.bin)
    char* ciphers_bench_fn = malloc(strlen(name) + sizeof(CIPHERS_BIN) + 1);
    strcpy(ciphers_bench_fn, name);
    strcat(ciphers_bench_fn, CIPHERS_BIN);
    FILE *ciphers_bench_f = fopen(ciphers_bench_fn, "wb");
    free(ciphers_bench_fn);
    if (ciphers_bench_f == NULL) {
        printf("Error while opening ciphers file\n");
        return -1;
    }

    // get number of records
    fseek(plains_f, 0, SEEK_END);
    int nbRecords = ftell(plains_f) / 16;
    fseek(plains_f, 0, SEEK_SET);

    // compute AES and save results
    uint8_t plain[16];
    uint8_t key[16];
    uint8_t cipher[16];
    for (int idx=0; idx < nbRecords; idx++) {
        int status = fread(plain, 16, 1, plains_f);
        if (status != 1) {
            printf("Error while reading plains file\n");
            return -1;
        }
        status = fread(key, 16, 1, keys_f);
        if (status != 1) {
            printf("Error while reading keys file\n");
            return -1;
        }

        aes128_cipher(plain, key, cipher);

        fwrite(cipher, 16, 1, ciphers_bench_f);
    }
    fclose(plains_f);
    fclose(keys_f);
    fclose(ciphers_bench_f);

    return nbRecords;
}

