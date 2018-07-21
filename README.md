## Basic information

A proof-of-concept program for lightweight fingerprints for fast approximate keyword matching using bitwise operations. Authors: Aleksander Cisłak, Szymon Grabowski.

A preprint is available at the following link: https://arxiv.org/abs/1711.08475

## Compilation

Add Boost library to the path for compilation by setting `BOOST_DIR` in the makefile. Requires Boost program options module to be compiled for static linking.

Type `make` for optimized compile.
Comment out `OPTFLAGS` in the makefile in order to disable optimization.

Tested with gcc 64-bit 7.2.0 and Boost 1.67.0 (the latter is not performance-critical, used only for parameter and data parsing and formatting) on Ubuntu 17.10 Linux version 4.13.0-36 64-bit.

## Usage

Basic usage: `./fingerprints [options] <input dictionary file> <input pattern file>`

Input dictionary file (positional parameter 1 or named parameter `-i` or `--in-dict-file`) should contain the list of words, separated with newline characters.
Input pattern file (positional parameter 2 or named parameter `-I` or `--in-pattern-file`) should contain the list of patterns, separated with newline characters.
Attached as part of this package is a script `run_all.sh`, which allows for processing multiple input dictionary and pattern files.

Unit tests are located in the `unit_tests` folder and they can be run by issuing the `make run` command in that folder.

#### Command-line parameter description

Short name | Long name                | Parameter description
---------- | ------------------------ | ---------------------
`-d`       | `--dump`                 | dump input files and params info with elapsed time and throughput to output file (useful for testing)
`-D`       | `--distance arg`         | distance metric: 0 -> Hamming, 1 -> Levenshtein (default = 0)
`-f`       | `--fingerprint-type arg` | fingerprint type: -1 -> no fingerprints, 0 -> occurrence, 1 -> count, 2 -> position, 3 -> occurrence halved (default = 0)
`-h`       | `--help`                 | display help message
`-i`       | `--in-dict-file arg`     | input dictionary file path (positional arg 1)
`-I`       | `--in-pattern-file arg`  | input pattern file path (positional arg 2)
`-k`       | `--approx arg`           | perform approximate search (Hamming or Levenshtein) for k errors
`-l`       | `--letters-type arg`     | letters type: 0 -> common, 1 -> mixed, 2 -> rare (default = 0)
`-o`       | `--out-file arg`         | output file path
`-p`       | `--pattern-count arg`    | maximum number of patterns read from top of the pattern file
&nbsp;     | `--pattern-size arg`     | if set, only patterns of this size (letter count) will be read from the pattern file
`-s`       | `--separator arg`        | input data (dictionary and patterns) separator
`-v`       | `--version`              | display version info
`-w`       | `--word-count arg`       | maximum number of words read from top of the dictionary file
