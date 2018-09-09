#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <vector>

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

    /** Dump fingerprint construction time. */
    bool dumpConstruction = false;

    /** Dump input files and params info with elapsed and throughput to output file. Cmd arg -d. */
    bool dumpToFile = false;

    /** Distance type: ham (Hamming), lev (Levenshtein). Cmd arg -D. */
    std::string distanceType;

    /** Fingerprint type: none, occ (occurrence), occhalved (occurrence halved), count, pos (position). Cmd arg -f. */
    std::string fingerprintType;

    /** Letters type: common, mixed, rare. Cmd arg -l. */
    std::string lettersType;

    /** Number of iterations per pattern lookup. */
    int nIter;

    /** Number of errors for approximate search (Hamming or Levenshtein). Cmd arg -k. */
    int kApprox = noValue;
   
    /** Maximum number of patterns read from top of the patterns file. Cmd arg -p. */
    int nPatterns = noValue;
    /** If set, only patterns of this size (letter count) will be read from the pattern file. */
    int patternSize = noValue;
    /** Maximum number of words read from top of the dictionary file. Cmd arg -w. */
    int nWords = noValue;

    /** Input data (dictionary and patterns) separator. */
    std::string separator = "\n";

    /** Input dictionary file path (positional arg 1). */
    std::string inDictFile;
    /** Input pattern file path (positional arg 2). */
    std::string inPatternFile;

    /** Output file path. */
    std::string outFile;

    /*
     *** CONSTANTS
     */

    /** Returned from main on failure. */
    static constexpr int errorExitCode = 1;
    /** Indicates that a given non-negative integer is not set. */
    static constexpr int noValue = -1;

    /** Current version: major.minor.patch */
    const std::string versionInfo = "fingerprints v1.2.0";
    const std::string usageInfoString = "[options] <input dictionary file> <input pattern file>";
};

} // namespace fingerprints

#endif // PARAMS_HPP
