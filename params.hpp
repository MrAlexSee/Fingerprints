#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <vector>

using namespace std;

namespace fingerprints
{

struct Params
{
    // COMMAND-LINE PARAMS
    // These parameters are set by handleParams() in main.cpp after parsing command line args.

    bool dumpToFile = false; // Dump input file info and throughput to output file (outFile). Cmd arg -d.

    int kApprox = noValue; // Number of errors for approximate search (Hamming). noValue = perform exact search. Cmd arg -k.
    int nPatterns = noValue; // Maximum number of patterns read from top of the patterns file; noValue = ignore the pattern count limit. Cmd arg -p.

    string inDictFile = ""; // Input dictionary file path (positional arg 1).
    string inPatternFile = ""; // Input pattern file path (positional arg 2).

    string outFile = "res.txt"; // Output file path.

    // CONSTANTS
    static constexpr int errorExitCode = 1; // Returned from main on failure.
    static constexpr int noValue = -1; // Indicates that a given non-negative integer is not set.

    const string versionInfo = "fingerprints v0.1.0"; // Current version: major.minor.patch
    const string usageInfoString = "[options] <input text file> <input pattern file>";
};

} // namespace fingerprints

#endif // PARAMS_HPP
