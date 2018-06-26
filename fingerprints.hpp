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
    void preprocess(const vector<string> &words);
    void test(const vector<string> &patterns);

    float getElapsedUs() const { return elapsedUs; }

private:
    FING_T calcFingerprint(const char *str, size_t size) const;
    FING_T calcFingerprintOcc(const char *str, size_t size) const;

    unsigned char *charsMap = nullptr;

    float elapsedUs = 0.0f;

    // CONSTANTS follow

    // Indicates that the character is not stored in a fingerprint.
    static const unsigned char noCharIndex = 255;

    const string commonChars16 = "etaoinshrdlcumwf";
    const string rareChars16 = "zqxjkvbpygfwmucl";
    const string mixedChars16 = "etaoinshzqxjkvbp";

    const string commonChars8 = "etaoinsh";
    const string rareChars8 = "zqxjkvbp";
    const string mixedChars8 = "etaokvbp";
};

} // namespace fingerprints

#endif // FINGERPRINTS_HPP
