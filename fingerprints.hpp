#ifndef FINGERPRINTS_HPP
#define FINGERPRINTS_HPP

#include <string>
#include <vector>

using namespace std;

namespace fingerprints
{

template<typename SKETCH_T>
class Fingerprints
{
public:
    void preprocess(const vector<string> &words);
    void test(const vector<string> &patterns);

    float getElapsedUs() const { return elapsedUs; }

private:
    float elapsedUs = 0.0f;

    const string commonChars16 = "etaoinshrdlcumwf";
    const string rareChars16 = "zqxjkvbpygfwmucl";
    const string mixedChars16 = "etaoinshzqxjkvbp";

    const string commonChars8 = "etaoinsh";
    const string rareChars8 = "zqxjkvbp";
    const string mixedChars8 = "etaokvbp";
};

} // namespace fingerprints

#endif // FINGERPRINTS_HPP
