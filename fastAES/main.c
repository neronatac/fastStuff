#include <string.h>
#include <argparse.h>
#include <stdio.h>
#include "aes.h"
#include "cmd_gen.h"
#include "cmd_bench.h"
#include "utils.h"


#define ARRAY_SIZE(x) (int)(sizeof(x) / sizeof(x[0]))


struct cmd_struct {
    const char *cmd;
    const char *desc;
    int (*fn) (int, const char **);
};

static struct cmd_struct commands[] = {
    {"gen", "generates plain/key/cipher files", cmd_gen},
    {"bench", "uses plain/key files to create a cipher file and compare results with reference file", cmd_bench},
};


int main(int argc, const char **argv) {
    printf("fastAES program\n");

    int support = is_aesni_supported();
    if(!support) {
        printf("AES-NI not supported!\n");
        return -1;
    }

    // handle arguments
    struct argparse argparse;
    struct argparse_option options[] = {
        OPT_END(),
    };
    argparse_init(&argparse, options, usages, ARGPARSE_STOP_AT_NON_OPTION);
    argc = argparse_parse(&argparse, argc, argv);
    if (argc < 1) {
        argparse_usage(&argparse);
        printf("List of commands:\n");
        for (int i=0; i < ARRAY_SIZE(commands); i++) {
            printf("\t%s: %s\n", commands[i].cmd, commands[i].desc);
        }
        return -1;
    }
    
    // run command
    struct cmd_struct *cmd = NULL;
    for (int i = 0; i < ARRAY_SIZE(commands); i++) {
        if (!strcmp(commands[i].cmd, argv[0])) {
            cmd = &commands[i];
        }
    }
    if (cmd) {
        printf("%s command\n", cmd->cmd);
        return cmd->fn(argc, argv);
    }

    return -1;
}
