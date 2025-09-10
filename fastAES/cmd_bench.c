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
    const char *chunk_size_str = NULL;

    // parameters definition
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING('n', "name", &name, "name of files to treat", NULL, 0, 0),
        OPT_STRING('c', "chunk-size", &chunk_size_str, "chunk size in bytes, RAM usage is 3x greater. Must be multiple of 16. Can use 'k', 'M' or 'G' if necessary. DEFAULT: 1M", NULL, 0, 0),
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
    uint64_t chunk_size = 1024*1024; // 1 MB
    if (chunk_size_str != NULL) {
        if (parse_long_with_suffix(chunk_size_str, &chunk_size) != 0) {
            argparse_usage(&argparse);
            printf("Error while parsing chunk size\n");
            return -1;
        }
    }
    printf("Chunk size: %ld\n", chunk_size);
    if (chunk_size % 16 != 0) {
        argparse_usage(&argparse);
        printf("Chunk size must be a multiple of 16\n");
        return -1;
    }

    printf("Benchmark on %s files...\n", name);
    time_t begin_wall_time = time(NULL);
    clock_t begin_cpu_time = clock();

    // do the thing
    int nbRecords = aes_treat_files(name, chunk_size);
    
    if (nbRecords < 0) {
        printf("Something went wrong during 'aes_treat_files' function");
        return nbRecords;
    }

    // get elapsed time
    clock_t end_cpu_time = clock();
    time_t end_wall_time = time(NULL);
    double elapsed_cpu = (double)(end_cpu_time - begin_cpu_time) / CLOCKS_PER_SEC;
    double elapsed = difftime(end_wall_time, begin_wall_time);
    printf("Elapsed CPU time: %f seconds\n", elapsed_cpu);
    printf("Elapsed wall time: %f seconds\n", elapsed);
    printf("%d AES/s\n", (int)(nbRecords / elapsed));
    printf("1 AES in %f ns\n", elapsed / (nbRecords / 1e9));

    // check results
    printf("Checking results...\n");

    char *ciphers_bench_fn = malloc(strlen(name) + sizeof(CIPHERS_BIN) + 1);
    strcpy(ciphers_bench_fn, name);
    strcat(ciphers_bench_fn, CIPHERS_BIN);
    FILE *ciphers_bench_f = fopen(ciphers_bench_fn, "rb");
    free(ciphers_bench_fn);
    if (ciphers_bench_f == NULL) {
        printf("Error while opening ciphers file\n");
        return -1;
    }

    char* ciphers_fn = malloc(strlen(name) + sizeof(CIPHERS_REF_BIN) + 1);
    strcpy(ciphers_fn, name);
    strcat(ciphers_fn, CIPHERS_REF_BIN);
    FILE *ciphers_f = fopen(ciphers_fn, "rb");
    free(ciphers_fn);
    if (ciphers_f == NULL) {
        printf("Error while opening ciphers_ref file\n");
        return -1;
    }

    int idx;
    int is_good = 1;
    uint8_t cipher_bench[16];
    uint8_t cipher[16];
    for (idx=0; idx < nbRecords; idx++) {
        int status = fread(cipher, 16, 1, ciphers_f);
        if (status != 1) {
            printf("Error while reading ciphers_ref file\n");
            return -1;
        }
        status = fread(cipher_bench, 16, 1, ciphers_bench_f);
        if (status != 1) {
            printf("Error while reading ciphers file\n");
            return -1;
        }
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
