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

## Notes on performance

Performances are measured using a set of 1 billion AES entries (16 GB files) and the `bench` command.

Intel VTune software is used to profile execution.

### V1 - first implementation, naive

| Metric | Result |
| :-: | -: |
| CPU time | 100 s |
| Wall time | 102 s |
| AES/s | 9_803_921 |
| 1 AES | 102 ns |

The first implemented version is quite naive. However, AES-NI instructions are already used as it was the purpose of this project.

Hotspots analysis from VTune showed that `fread` and `fwrite` calls take respectively about 27% and 21% of execution time of `aes_treat_files` function!