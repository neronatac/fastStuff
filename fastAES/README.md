# fastAES

Fast AES128 implementation. The goal is to read/write files and do AES128 as fast as possible.

It's a CLI program. It can generate (`gen` command) binary files containing keys, plaintexts and ciphertexts. These files are meant to be used in the `bench` command: it takes plaintexts and keys files and generates a second ciphertext file. Then, ciphertexts are compared to check their validity.



## Dependencies:
- toolchain:
    - make
    - gcc
    - nasm
- libraries:
    - argparse: https://github.com/cofyc/argparse