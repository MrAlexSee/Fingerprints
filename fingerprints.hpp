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
    /** Constructs a fingerprints object for [fingerprintType] and [lettersType].
     * Consult params.hpp for more information regarding the parameters. */
    Fingerprints(int fingerprintType, int lettersType);
    ~Fingerprints();

    /** Constructs an array which stores [words] together with their corresponding fingerprints. */
    void preprocess(vector<string> words);
    /** Performs approximate matching for [patterns] and [k] errors. Returns the total number of matches.
     * Sets elapsedUs to time elapsed during this matching. */
    int test(const vector<string> &patterns, int k);

    float getElapsedUs() const { return elapsedUs; }

private:
    /*
     *** INITIALIZIATION
     */

    void initNErrorsLUT();
    
    void initCharsMap(int fingerprintType, int lettersType);
    string getCharList(size_t nChars, int lettersType) const;

    /** Calculates mismatches LUT (nMismatchesLUT) for occurrence fingerprints. */
    void calcOccNMismatchesLUT();
    /** Calculates mismatches LUT (nMismatchesLUT) for count fingerprints. */
    void calcCountNMismatchesLUT();

    /** Returns the total size of an array for words from [words] and corresponding fingerprints.
     * Calculates a count for each word size and stores it in [wordCountsBySize]
     * (passed array must be of size maxWordSize + 1). */
    static size_t calcTotalSize(const vector<string> &words, size_t *wordCountsBySize);

    /*
     *** FINGERPRINT CALCULATION
     */

    /** Returns a fingerprint of the current type, set to one of the specific functions listed below. */
    function<FING_T(const char *, size_t)> calcFingerprintFun;

    /** Returns an occurrence fingerprint for string [str] having [size] chars. */
    FING_T calcFingerprintOcc(const char *str, size_t size) const;
    /** Returns a count fingerprint for string [str] having [size] chars. */
    FING_T calcFingerprintCount(const char *str, size_t size) const;

    /** Returns the Hamming weight for number [n]. */
    static unsigned int calcHammingWeight(unsigned int n);
    /** Returns the number of errors resulting from comparing fingerprints [f1] and [f2]. */
    unsigned char calcNErrors(FING_T f1, FING_T f2) const;

    /** Returns true if Hamming distance between [str1] and [str2] both of [size] is at most [k] (i.e. <= k). */
    static bool isHamAMK(const char *str1, const char *str2, size_t size, int k);

    /*
     *** CONSTANTS
     */

    static constexpr size_t maxWordSize = 255;
    static constexpr size_t charsMapSize = 255;

    /** Indicates that a character is not stored in a fingerprint. */
    static constexpr unsigned char noCharIndex = 255;

    /*
     *** ARRAYS, MAPS, AND LOOKUP TABLES
     */

    /** Stores contiguously pairs (fingerprint, word) sorted by word size. */
    char *fingArray = nullptr;
    /** Points to the beginning of each word size bracket in fingArray. */
    char *fingArrayEntries[maxWordSize + 1];

    /** Maps chars to their positions in fingerprints. */
    unsigned char *charsMap = nullptr;

    /** Stores the minimum number of actual errors for a given number of fingerprint mismatches. */
    unsigned char *nErrorsLUT = nullptr;
    /** Stores the number of fingerprint mismatches resulting from fingerprint comparison. */
    unsigned char *nMismatchesLUT = nullptr;

    /** Elapsed time in microseconds. */
    float elapsedUs = 0.0f;

    /*
     *** FINGERPRINT LETTER COLLECTIONS
     */

    const string commonChars16 = "etaoinshrdlcumwf";
    const string mixedChars16 = "etaoinshzqxjkvbp";
    const string rareChars16 = "zqxjkvbpygfwmucl";

    const string commonChars8 = "etaoinsh";
    const string mixedChars8 = "etaokvbp";
    const string rareChars8 = "zqxjkvbp";

    FINGERPRINTS_WHITEBOX
};

} // namespace fingerprints

#endif // FINGERPRINTS_HPP
