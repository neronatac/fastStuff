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

For each version, a benchmark will be done and following measures will be displayed:
- CPU time: amount of time running on CPU
- Wall time: real amount of time (including when the program is not running due to scheduling)
- AES/s: equivalent number of AES computations per second (computed using wall time)
- 1 AES: equivalent mean time to compute a single AES (computed using wall time)

Intel VTune software is used to profile execution.

### Hardware specifications

All benchmarks have been done on this hardware:
- OS: Linux 6.14
- processor: Intel Core Ultra 5 226V
- motherboard: ASUSTeK S5406SA
- RAM: 16 GB
- SSD

### V1 - first implementation, naive

The first implemented version is quite naive:
- files are opened
- in a for loop:
    - 16 bytes are read from plains file
    - 16 bytes are read from keys file
    - AES is done
    - 16 bytes are written into ciphers file

However, AES-NI instructions are already used as it was the purpose of this project.

|  Metric   |    Result |
|:---------:|----------:|
| CPU time  |     100 s |
| Wall time |     102 s |
|   AES/s   | 9_803_921 |
|   1 AES   |    102 ns |

Hotspots analysis from VTune showed that `fread` and `fwrite` calls take respectively about 27% and 21% of execution time of `aes_treat_files` function!

### V2 - read/write files by chunks

To avoid wasting time doing multiple calls to `fread` and `fwrite`, in this version, instead of reading a single key/plaintext, 3 chunks are allocated in memory. Then, they are filled with a single `fread` call. AES are done in a loop and when chunk is entirely handled, a single call to `fwrite` is done.

|  Metric   | Result (chunck size = 4GB) | Result (chunck size = 1GB) | Result (chunck size = 100MB) | Result (chunck size = 10MB) |
|:---------:|---------------------------:|---------------------------:|-----------------------------:|----------------------------:|
| CPU time  |                       59 s |                       51 s |                         46 s |                        46 s |
| Wall time |                       95 s |                       81 s |                         79 s |                        78 s |
|   AES/s   |                 10_526_315 |                 12_345_679 |                   12_658_227 |                  12_820_512 |
|   1 AES   |                      95 ns |                      81 ns |                        79 ns |                       78 ns |

|  Metric   | Result (chunck size = 1MB) | Result (chunck size = 100kB) | Result (chunck size = 10kB) | Result (chunck size = 1kB) |
|:---------:|---------------------------:|-----------------------------:|----------------------------:|---------------------------:|
| CPU time  |                       45 s |                         42 s |                        47 s |                       53 s |
| Wall time |                       76 s |                         53 s |                        55 s |                       59 s |
|   AES/s   |                 13_157_894 |                   18_867_924 |                  18_181_818 |                 16_949_152 |
|   1 AES   |                      76 ns |                        53 ns |                       55 ns |                      59 ns |

Best results are for a chunk size of 100 kB. L1 cache of the CPU is: 4 x 48 kB, 12-way associative, 64-bytes line size. 2*48 kB is almost 100 kB, there may be a link.

In VTune, hotspots analysis gives interesting results. For example (chunks of 1 GB):
- elapsed time: 121s
- top hotspots:
    - aes128_cipher: CPU time: 29 s
    - fread: CPU time: 24 s
    - all others: CPU time: < 1 s
- total CPU time: 53 s

It lacks 68 s!

It probably corresponds to the DMA transfers between chunk buffers in RAM and SSD.