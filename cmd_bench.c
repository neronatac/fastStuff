#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <argparse.h>
#include <time.h>
#include <string.h>
#include "cmd_bench.h"
#include "aes.h"
#include "utils.h"

int cmd_bench(int argc, const char **argv) {
    const char *name = NULL;

    // parameters definition
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING('n', "name", &name, "name of files to treat", NULL, 0, 0),
        OPT_END(),
    };

    // check and get parameters
    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argc = argparse_parse(&argparse, argc, argv);
    if (name == NULL) {
        argparse_usage(&argparse);
        printf("Name must be defined\n");
        return -1;
    }

    printf("Benchmark on %s files...\n", name);
    clock_t begin_time = clock();

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
    char* ciphers_bench_fn = malloc(strlen(name) + sizeof(CIPHERS_BENCH_BIN) + 1);
    strcpy(ciphers_bench_fn, name);
    strcat(ciphers_bench_fn, CIPHERS_BENCH_BIN);
    FILE *ciphers_bench_f = fopen(ciphers_bench_fn, "wb");
    free(ciphers_bench_fn);
    if (ciphers_bench_f == NULL) {
        printf("Error while opening ciphers_bench file\n");
        return -1;
    }

    // get number of records
    fseek(plains_f, 0, SEEK_END);
    unsigned int nbRecords = ftell(plains_f) / 16;
    fseek(plains_f, 0, SEEK_SET);

    // compute AES and save results
    uint8_t plain[16];
    uint8_t key[16];
    uint8_t cipher[16];
    for (unsigned int idx=0; idx < nbRecords; idx++) {
        fread(plain, 16, 1, plains_f);
        fread(key, 16, 1, keys_f);

        aes128_cipher(plain, key, cipher);

        fwrite(cipher, 16, 1, ciphers_bench_f);
    }
    fclose(plains_f);
    fclose(keys_f);
    fclose(ciphers_bench_f);

    // get elapsed time
    clock_t end_time = clock();
    double elapsed = (double)(end_time - begin_time) / CLOCKS_PER_SEC;
    printf("Elapsed time: %f seconds\n", elapsed);
    printf("%d AES/s\n", (int)(nbRecords / elapsed));
    printf("1 AES in %d s\n", elapsed / (double)nbRecords);

    // check results
    printf("Checking results...\n");

    ciphers_bench_fn = malloc(strlen(name) + sizeof(CIPHERS_BENCH_BIN) + 1);
    strcpy(ciphers_bench_fn, name);
    strcat(ciphers_bench_fn, CIPHERS_BENCH_BIN);
    ciphers_bench_f = fopen(ciphers_bench_fn, "rb");
    free(ciphers_bench_fn);
    if (ciphers_bench_f == NULL) {
        printf("Error while opening ciphers_bench file\n");
        return -1;
    }

    char* ciphers_fn = malloc(strlen(name) + sizeof(CIPHERS_BIN) + 1);
    strcpy(ciphers_fn, name);
    strcat(ciphers_fn, CIPHERS_BIN);
    FILE *ciphers_f = fopen(ciphers_fn, "rb");
    free(ciphers_fn);
    if (ciphers_f == NULL) {
        printf("Error while opening ciphers file\n");
        return -1;
    }

    unsigned int idx;
    int is_good = 1;
    uint8_t cipher_bench[16];
    for (idx=0; idx < nbRecords; idx++) {
        fread(cipher, 16, 1, ciphers_f);
        fread(cipher_bench, 16, 1, ciphers_bench_f);
        if (memcmp(cipher, cipher_bench, 16) != 0) {
            is_good = 0;
            break;
        }
    }
    if (is_good == 0) {
        printf("Error found at result number %d\n", idx);
    } else {
        printf("All good!\n");
    }


    return 0;
}
