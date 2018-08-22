#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <vector>

using namespace std;

namespace fingerprints
{

struct Params
{
    /*
     *** COMMAND-LINE PARAMS
     */

    // These parameters are set by handleParams() in main.cpp after parsing command line args.

    /** Calculate percentages of rejected words instead of measuring time. */
    bool calcRejection = false;

    /** Dump input files and params info with elapsed and throughput to output file. Cmd arg -d. */
    bool dumpToFile = false;

    /** Distance type: 0 -> Hamming, 1 -> Levenshtein. Cmd arg -D. */
    int distanceType = 0;
    /** Fingerprint type: -1 -> no fingerprints, 0 -> occurrence, 1 -> count, 2 -> position, 3 -> occurrence halved. Cmd arg -f. */
    int fingerprintType = 0;
    /** Letters type: 0 -> common, 1 -> mixed, 2 -> rare. Cmd arg -l. */
    int lettersType = 0;

    /** Number of errors for approximate search (Hamming or Levenshtein). Cmd arg -k. */
    int kApprox = noValue;
   
    /** Maximum number of patterns read from top of the patterns file. Cmd arg -p. */
    int nPatterns = noValue;
    /** If set, only patterns of this size (letter count) will be read from the pattern file. */
    int patternSize = noValue;
    /** Maximum number of words read from top of the dictionary file. Cmd arg -w. */
    int nWords = noValue;

    /** Number of iterations per pattern lookup. */
    int nIter = 1;

    /** Input dictionary file path (positional arg 1). */
    string inDictFile = "";
    /** Input pattern file path (positional arg 2). */
    string inPatternFile = "";

    /** Input data (dictionary and patterns) separator. */
    string separator = "\n";
    /** Output file path. */
    string outFile = "res.txt";

    /*
     *** CONSTANTS
     */

    /** Returned from main on failure. */
    static constexpr int errorExitCode = 1;
    /** Indicates that a given non-negative integer is not set. */
    static constexpr int noValue = -1;

    /** Current version: major.minor.patch */
    const string versionInfo = "fingerprints v1.0.0";
    const string usageInfoString = "[options] <input dictionary file> <input pattern file>";
};

} // namespace fingerprints

#endif // PARAMS_HPP
