#ifndef FINGERPRINTS_HPP
#define FINGERPRINTS_HPP

#include <functional>
#include <string>
#include <vector>

#ifndef FINGERPRINTS_WHITEBOX
#define FINGERPRINTS_WHITEBOX
#endif

using namespace std;

namespace fingerprints
{

template<typename FING_T>
class Fingerprints
{
public:
    /** Constructs a fingerprints object for [distanceType], [fingerprintType], and [lettersType].
     * Consult params.hpp for more information regarding the parameters. */
    Fingerprints(int distanceType, int fingerprintType, int lettersType);
    ~Fingerprints();

    /** Constructs an array which stores [words],
     * if [useFingerprints] is true together with their corresponding fingerprints. */
    void preprocess(const vector<string> &words);

    /** Performs approximate matching for [patterns] and [k] errors, iterates [nIter] times.
     * Returns the total number of matches. Sets elapsedUs to time elapsed during this matching. */
    int test(const vector<string> &patterns, int k, int nIter);

    /** Tests [patterns] for [k] errors using fingerprints.
     * Returns the fraction of words which were rejected by fingerprints. */
    float testRejection(const vector<string> &patterns, int k);

    float getElapsedUs() const { return elapsedUs; }

    enum class DistanceType { Hamming = 0, Leven = 1 };
    enum class FingerprintType { NoFing = -1, Occ = 0, Count = 1, Pos = 2, OccHalved = 3 };
    enum class LettersType { Common = 0, Mixed = 1, Rare = 2 };

private:
    /*
     *** INITIALIZIATION
     */

    /** Constructs an array which stores [words] together with their corresponding fingerprints. */
    void preprocessFingerprints(vector<string> words);
    /** Constructs an array which stores only [words]. */
    void preprocessWords(vector<string> words);

    /** Performs approximate matching for [patterns] and [k] errors using fingerprints for Hamming distance.
     * Returns the total number of matches. Sets elapsedUs to time elapsed during this matching. */
    int testFingerprintsHamming(const vector<string> &patterns, int k);
    /** Performs approximate matching for [patterns] and [k] errors using fingerprints for Levenshtein distance.
     * Returns the total number of matches. Sets elapsedUs to time elapsed during this matching. */
    int testFingerprintsLeven(const vector<string> &patterns, int k);

    /** Performs approximate matching for [patterns] and [k] errors without fingerprints for Hamming distance.
     * Returns the total number of matches. Sets elapsedUs to time elapsed during this matching. */
    int testWordsHamming(const vector<string> &patterns, int k);
    /** Performs approximate matching for [patterns] and [k] errors without fingerprints for Levenshtein distance.
     * Returns the total number of matches. Sets elapsedUs to time elapsed during this matching. */
    int testWordsLeven(const vector<string> &patterns, int k);

    /** Tests [patterns] for [k] errors using fingerprints for Hamming distance.
     * Returns the fraction of words which were rejected by fingerprints. */
    float testRejectionHamming(const vector<string> &patterns, int k);

    /** Tests [patterns] for [k] errors using fingerprints for Levenshtein distance.
     * Returns the fraction of words which were rejected by fingerprints. */
    float testRejectionLeven(const vector<string> &patterns, int k);

    /** Initializes a lookup table for true number of errors based on fingerprints errors. */
    void initNErrorsLUT();
    
    /** Initializes the character map for [fingerprintType] (occurrence, count) and [lettersType] (common, mixed, rare). */
    void initCharsMap(int fingerprintType, int lettersType);
    /** Initializes the character list for position fingerprint and [lettersType] (common, mixed, rare). */
    void initCharList(int lettersType);
    
    /** Returns the character list for [nChars] count and [lettersType] (common, mixed, rare). */
    string getCharList(size_t nChars, int lettersType) const;

    /** Calculates mismatches LUT (nMismatchesLUT) for occurrence fingerprints. */
    void calcOccNMismatchesLUT();
    /** Calculates mismatches LUT (nMismatchesLUT) for count fingerprints. */
    void calcCountNMismatchesLUT();
    /** Calculates mismatches LUT (nMismatchesLUT) for position fingerprints. */
    void calcPosNMismatchesLUT();

    /** Returns the total size of an array for words from [words] and corresponding fingerprints.
     * Calculates a count for each word size and stores it in [wordCountsBySize]
     * (passed array must be of size maxWordSize + 1). */
    static size_t calcTotalSize(const vector<string> &words, size_t *wordCountsBySize);

    /** Set to false if the user selected the mode without fingeprints (where only the words are stored). */
    bool useFingerprints = true;
    /** Set to false if the user selected the Levenshtein distance. */
    bool useHamming = true;

    /*
     *** FINGERPRINT CALCULATION
     */

    /** Returns a fingerprint of the current type for a string.
     * Set to one of the specific functions listed below. */
    function<FING_T(const char *, size_t)> calcFingerprintFun;

    /** Returns an occurrence fingerprint for string [str] having [size] chars. */
    FING_T calcFingerprintOcc(const char *str, size_t size) const;
    /** Returns a count fingerprint for string [str] having [size] chars. */
    FING_T calcFingerprintCount(const char *str, size_t size) const;
    /** Returns a position fingerprint for string [str] having [size] chars. */
    FING_T calcFingerprintPos(const char *str, size_t size) const;
    /** Returns an occurrence halved fingerprint for string [str] having [size] chars. */
    FING_T calcFingerprintOccHalved(const char *str, size_t size) const;

    /** Returns the Hamming weight for number [n]. */
    static unsigned int calcHammingWeight(unsigned int n);
    /** Returns the number of errors resulting from comparing fingerprints [f1] and [f2]. */
    unsigned char calcNErrors(FING_T f1, FING_T f2) const;

    /** Returns true if Hamming distance between [str1] and [str2] both of [size] is at most [k] (i.e. <= k). */
    static bool isHamAMK(const char *str1, const char *str2, const size_t size, const int k);

    /** Returns true if Levenshtein distance between [str1] of [size1] and [str2] of [size2] is at most [k] (i.e. <= k).
     * Uses the 2k + 1 strip. */
    bool isLevAMK(const char *str1, size_t size1, const char *str2, const size_t size2, const int k);

    /** Elapsed time in microseconds. */
    float elapsedUs = 0.0f;

    /*
     *** CONSTANTS
     */

    static constexpr size_t maxWordSize = 255;
    static constexpr size_t charsMapSize = 255;

    /** Indicates that a character is not stored in a fingerprint. */
    static constexpr unsigned char noCharIndex = 255;

    /** Number of bits per position in a position fingerprint. */
    static constexpr size_t nBitsPerPos = 3;
    
    /*
     *** ARRAYS, MAPS, AND LOOKUP TABLES
     */

    /** Stores contiguously pairs (fingerprint, word) sorted by word size. */
    char *fingArray = nullptr;
    /** Points to the beginning of each word size bracket in fingArray. */
    char *fingArrayEntries[maxWordSize + 2];

    /** Maps chars to their positions in fingerprints, used for occurrence and count fingerprints. */
    unsigned char *charsMap = nullptr;

    /** Character list, used for position fingerprints. */
    unsigned char *charList = nullptr;

    /** Stores the minimum number of actual errors for a given number of fingerprint mismatches. */
    unsigned char *nErrorsLUT = nullptr;
    /** Stores the number of fingerprint mismatches resulting from fingerprint comparison. */
    unsigned char *nMismatchesLUT = nullptr;

    /** Stores intermediate results of Levenshtein distance calculation (previous array). */
    int *levV0 = nullptr;
    /** Stores intermediate results of Levenshtein distance calculation (current array). */
    int *levV1 = nullptr;

    /*
     *** FINGERPRINT LETTER COLLECTIONS
     */

    const string engCommonLetters16 = "etaoinshrdlcumwf";
    const string engMixedLetters16 = "etaoinshzqxjkvbp";
    const string engRareLetters16 = "zqxjkvbpygfwmucl";

    const string engCommonLetters8 = "etaoinsh";
    const string engMixedLetters8 = "etaokvbp";
    const string engRareLetters8 = "zqxjkvbp";

    // These collections use 5 letters for a position fingerprint 
    // and a single, additional character for occurrence.
    const string engCommonLettersPos5 = "etaoin";
    const string engMixedLettersPos5 = "etakvb";
    const string engRareLettersPos5 = "zqxjkv";

    FINGERPRINTS_WHITEBOX
};

} // namespace fingerprints

#endif // FINGERPRINTS_HPP
