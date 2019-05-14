## Basic information

A proof-of-concept program for lightweight fingerprints for fast approximate keyword matching. Authors: Aleksander Cisłak, Szymon Grabowski.

Accepted for publication as *Lightweight Fingerprints for Fast Approximate Keyword Matching Using Bitwise Operations* in Computing and Informatics journal.
A preprint is available at the following link: https://arxiv.org/abs/1711.08475.

## Compilation

Add Boost library to the path for compilation by setting `BOOST_DIR` in the makefile. 
Requires Boost program options module to be compiled for static linking.
Requires support for the C++14 standard.

Type `make` for optimized compile.
Comment out `OPTFLAGS` in the makefile in order to disable optimization.

Tested with gcc 64-bit 7.2.0 and Boost 1.67.0 (the latter is not performance-critical, used only for parameter and data parsing and formatting) on Ubuntu 17.10 Linux version 4.13.0-36 64-bit.

A binary (compiled executable) for Linux is available in the release (file name `fingerprints`).

## Usage

Basic usage: `./fingerprints [options] <input dictionary file> <input pattern file>`

Input dictionary file (positional parameter 1 or named parameter `-i` or `--in-dict-file`) should contain the list of words, separated with newline characters.
Input pattern file (positional parameter 2 or named parameter `-I` or `--in-pattern-file`) should contain the list of patterns, separated with newline characters.
Attached as part of this package are scripts `test_real.sh` for testing on real-world data and `test_synth.sh` for testing on synthetic data.

* End-to-end tests are located in the `end_to_end_tests` folder and they can be run using the `run_tests.sh` script in that folder.
* Unit tests are located in the `unit_tests` folder and they can be run by issuing the `make run` command in that folder.
* The `scripts` directory contains some helpful Python 2 tools.

#### Command-line parameter description

Short name | Long name                | Parameter description
---------- | ------------------------ | ---------------------
&nbsp;     | `--calc-rejection`       | calculate percentages of rejected words instead of measuring time
`-d`       | `--dump`                 | dump input files and params info with elapsed time and throughput to output file (useful for testing)
&nbsp;     | `--dump-construction`    | dump fingerprint construction time
`-D`       | `--distance arg`         | distance metric: ham (Hamming), lev (Levenshtein) (default = ham)
`-f`       | `--fingerprint-type arg` | fingerprint type: none, occ (occurrence), occhalved (occurrence halved), count, pos (position) (default = occ)
`-h`       | `--help`                 | display help message
`-i`       | `--in-dict-file arg`     | input dictionary file path (positional arg 1)
`-I`       | `--in-pattern-file arg`  | input pattern file path (positional arg 2)
&nbsp;     | `--iter arg`             | number of iterations per pattern lookup (default = 1)
`-k`       | `--approx arg`           | perform approximate search (Hamming or Levenshtein) for k errors
`-l`       | `--letters-type arg`     | letters type: common, mixed, rare (default = common)
`-o`       | `--out-file arg`         | output file path (default = res.txt)
`-p`       | `--pattern-count arg`    | maximum number of patterns read from top of the pattern file (non-positive values are ignored)
&nbsp;     | `--pattern-size arg`     | if set, only patterns of this size (letter count) will be read from the pattern file (non-positive values are ignored)
`-s`       | `--separator arg`        | input data (dictionary and patterns) separator (default = newline)
`-v`       | `--version`              | display version info
`-w`       | `--word-count arg`       | maximum number of words read from top of the dictionary file (non-positive values are ignored)

#### Data files description

All files listed below are located in the `data` folder.

File name                        | Description
-------------------------------- | -------------------
`dict_english_200.txt`           | words extracted from the English 200 collection from the Pizza&Chili index, 815,935 words
`dict_iamerican_insane.txt`      | English dictionary, 350,518 words
`queries_english_200_8.txt`      | queries of length 8 with errors extracted from the corresponding dictionary, 10,000 words
`queries_iamerican_insane_8.txt` | queries of length 8 with errors extracted from the corresponding dictionary, 10,000 words
`queries_misspellings.txt`       | a list of common English misspellings, 4,278 words
`queries_urls_69.txt`            | queries of length 69 with errors extracted from a dictionary of real-world URL data, 10,000 words

References:

* Pizza&Chili: http://pizzachili.dcc.uchile.cl/texts/nlang/english.200MB.gz
* iamerican-insane: Ubuntu package
* English misspellings: https://en.wikipedia.org/wiki/Wikipedia:Lists_of_common_misspellings/For_machines
* URL dictionary: http://data.law.di.unimi.it/webdata/in-2004/in-2004.urls.gz
