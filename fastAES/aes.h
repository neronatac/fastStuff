#ifndef AES_H
#define AES_H

#include <stdint.h>


// prototypes of functions in aes.nasm

/**
 * Does an AES128 on plain buffer using key buffer.
 * Ciphertext is saved in cipher buffer.
 */
void aes128_cipher(uint8_t* plain, uint8_t* key, uint8_t* cipher) __attribute__((sysv_abi));


// prototypes of functions in aes.c

/**
 * Checks whether AES-NI extension is supported by the CPU.
 * Returns 1 if it does, 0 if not.
 */
int is_aesni_supported();

/**
 * Reads keys and plains files and produces a ciphers one.
 * Returns the number of treated records or a negative  number in case of error.
 */
int aes_treat_files(const char *name);


#endif  // AES_H