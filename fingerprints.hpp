#ifndef FINGERPRINTS_HPP
#define FINGERPRINTS_HPP

#include <string>
#include <vector>

using namespace std;

// 0 -> occurrence fingerprint
#define FING_TYPE 0

namespace fingerprints
{

template<typename FING_T>
class Fingerprints
{
public:
    Fingerprints();
    ~Fingerprints();

    void preprocess(vector<string> words);
    void test(const vector<string> &patterns);

    float getElapsedUs() const { return elapsedUs; }

private:
    void initNErrorsLUT();
    void initCharsMap();

    void calcOccSetBitsLUT();

    size_t calcWordsArraySizes(const vector<string> &words, size_t *sizes);

    FING_T calcFingerprint(const char *str, size_t size) const;
    FING_T calcFingerprintOcc(const char *str, size_t size) const;

    static unsigned int calcHammingWeight(unsigned int n);
    unsigned char calcNErrors(FING_T f1, FING_T f2) const;

    static constexpr size_t maxWordSize = 255;
    static constexpr size_t charsMapSize = 255;

    static constexpr unsigned char noCharIndex = 255; // Indicates that the character is not stored in a fingerprint.

    char *fingArray = nullptr;
    char *fingArrayEntries[maxWordSize];

    unsigned char *charsMap = nullptr;
    unsigned char *setBitsLUT = nullptr;
    unsigned char *nErrorsLUT = nullptr;

    float elapsedUs = 0.0f;

    // Fingerprint letter collections follow.

    const string commonChars16 = "etaoinshrdlcumwf";
    const string rareChars16 = "zqxjkvbpygfwmucl";
    const string mixedChars16 = "etaoinshzqxjkvbp";

    const string commonChars8 = "etaoinsh";
    const string rareChars8 = "zqxjkvbp";
    const string mixedChars8 = "etaokvbp";
};

} // namespace fingerprints

#endif // FINGERPRINTS_HPP
