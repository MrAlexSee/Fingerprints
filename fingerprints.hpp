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
    Fingerprints(int fingerprintType, int lettersType);
    ~Fingerprints();

    void preprocess(vector<string> words);
    int test(const vector<string> &patterns, int k);

    float getElapsedUs() const { return elapsedUs; }

private:
    /*
     *** INITIALIZIATION
     */

    void initNErrorsLUT();
    
    void initCharsMap(int fingerprintType, int lettersType);
    string getCharList(size_t nChars, int lettersType) const;

    void calcOccSetBitsLUT();

    static size_t calcTotalSize(const vector<string> &words, size_t *wordCountsBySize);

    /*
     *** FINGERPRINT CALCULATION
     */

    function<FING_T(const char *, size_t)> calcFingerprintFun;

    FING_T calcFingerprintOcc(const char *str, size_t size) const;

    static unsigned int calcHammingWeight(unsigned int n);
    unsigned char calcNErrors(FING_T f1, FING_T f2) const;

    static bool isHamAMK(const char *str1, const char *str2, size_t size, int k);

    /*
     *** CONSTANTS
     */

    static constexpr size_t maxWordSize = 255;
    static constexpr size_t charsMapSize = 255;

    static constexpr unsigned char noCharIndex = 255; // Indicates that the character is not stored in a fingerprint.

    /*
     *** ARRAYS, MAPS AND LOOKUP TABLES
     */

    char *fingArray = nullptr;
    // Points to the beginning of each word size bracket in fingArray.
    char *fingArrayEntries[maxWordSize + 1];

    unsigned char *nErrorsLUT = nullptr;

    unsigned char *charsMap = nullptr;
    unsigned char *setBitsLUT = nullptr;

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
