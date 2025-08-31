#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <argparse.h>
#include "cmd_gen.h"
#include "aes.h"
#include "utils.h"


int cmd_gen(int argc, const char **argv) {
    const char *name = NULL;
    int nbRecords = 0;

    // parameters definition
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING('n', "name", &name, "name of files to generate", NULL, 0, 0),
        OPT_INTEGER('s', "size", &nbRecords, "number of records to generate", NULL, 0, 0),
        OPT_END(),
    };

    // check and get parameters
    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argc = argparse_parse(&argparse, argc, argv);
    if (name == NULL || nbRecords <= 0) {
        argparse_usage(&argparse);
        printf("Both name and size must be defined\n");
        return -1;
    }

    printf("Generating %d records...\n", nbRecords);

    // create files
    char* plains_fn = malloc(strlen(name) + sizeof(PLAINS_BIN) + 1);
    strcpy(plains_fn, name);
    strcat(plains_fn, PLAINS_BIN);
    FILE *plains_f = fopen(plains_fn, "wb");
    free(plains_fn);
    if (plains_f == NULL) {
        printf("Error while opening plains file\n");
        return -1;
    }

    char* keys_fn = malloc(strlen(name) + sizeof(KEYS_BIN) + 1);
    strcpy(keys_fn, name);
    strcat(keys_fn, KEYS_BIN);
    FILE *keys_f = fopen(keys_fn, "wb");
    free(keys_fn);
    if (keys_f == NULL) {
        printf("Error while opening keys file\n");
        return -1;
    }

    char* ciphers_fn = malloc(strlen(name) + sizeof(CIPHERS_BIN) + 1);
    strcpy(ciphers_fn, name);
    strcat(ciphers_fn, CIPHERS_BIN);
    FILE *ciphers_f = fopen(ciphers_fn, "wb");
    free(ciphers_fn);
    if (ciphers_f == NULL) {
        printf("Error while opening ciphers file\n");
        return -1;
    }

    // create records
    uint8_t plain[16];
    uint8_t key[16];
    uint8_t cipher[16];
    for (unsigned int idx=0; idx < nbRecords; idx++){
        getrandom(plain, 16, 0);
        getrandom(key, 16, 0);

        aes128_cipher(plain, key, cipher);

        fwrite(plain, 16, 1, plains_f);
        fwrite(key, 16, 1, keys_f);
        fwrite(cipher, 16, 1, ciphers_f);

        if (idx % 131072 == 0) {  // 128k
            fflush(plains_f);
            fflush(keys_f);
            fflush(ciphers_f);
            printf("%d/%d\r", idx, nbRecords);
        }
    }

    // clean up
    fclose(plains_f);
    fclose(keys_f);
    fclose(ciphers_f);

    printf("Generated %d record!\n", nbRecords);

    return 0;
}