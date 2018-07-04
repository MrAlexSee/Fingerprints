#include <map>

#include "catch.hpp"
#include "repeat.hpp"

#include "fingerprints_whitebox.hpp"

#include "../fingerprints.hpp"
#include "../fingerprints.cpp"

#include "../helpers.hpp"

using namespace std;

namespace fingerprints
{

namespace
{

constexpr int maxWordSize = 255;

constexpr int maxNStrings = 100;
constexpr int stringSize = 50;

constexpr int maxK = 10;
constexpr int nHammingRepeats = 10;

}

using FING_T = uint16_t;

TEST_CASE("is initializing errors LUT correct", "[fingerprints]")
{
    Fingerprints<FING_T> fingerprints(0, 0);
    FingerprintsWhitebox::initNErrorsLUT(fingerprints);

    const unsigned char *nErrorsLUT = FingerprintsWhitebox::getNErrorsLUT(fingerprints);

    REQUIRE(nErrorsLUT[0] == 0);
    REQUIRE(nErrorsLUT[1] == 1);
    REQUIRE(nErrorsLUT[2] == 1);
    REQUIRE(nErrorsLUT[3] == 2);
    REQUIRE(nErrorsLUT[4] == 2);
    REQUIRE(nErrorsLUT[16] == 8);
}

TEST_CASE("is initializing chars map for 16 common letters correct", "[fingerprints]")
{
    Fingerprints<FING_T> fingerprints(0, 0);
    FingerprintsWhitebox::initCharsMap(fingerprints, 0, 0);

    const unsigned char *charsMap = FingerprintsWhitebox::getCharsMap(fingerprints);
    string letters = "etaoinshrdlcumwf";

    for (size_t i = 0; i < 16; ++i)
    {
        REQUIRE(charsMap[static_cast<size_t>(letters[i])] == i);
    }
}

TEST_CASE("is calculating mismatches LUT for occurrence fingerprints correct", "[fingerprints]")
{
    Fingerprints<FING_T> fingerprints(0, 0);
    FingerprintsWhitebox::calcOccNMismatchesLUT<FING_T>(fingerprints);

    const unsigned char *nMismatchesLUT = FingerprintsWhitebox::getNMismatchesLUT(fingerprints);
    
    // We check the mismatches for selected predefined results below.
    for (const auto &kv : map<size_t, size_t>({ { 0, 0 }, { 1, 1 }, { 2, 1 }, { 4, 1 }, { 5, 2 }, { 6, 2 }, { 7, 3 }, { 8, 1 } }))
    {
        REQUIRE(nMismatchesLUT[kv.first] == kv.second);
    }

    REQUIRE(nMismatchesLUT[0b000111010110] == 6);
}

TEST_CASE("is calculating mismatches LUT for count fingerprints correct", "[fingerprints]")
{
    Fingerprints<FING_T> fingerprints(1, 0);
    FingerprintsWhitebox::calcCountNMismatchesLUT<FING_T>(fingerprints);

    const unsigned char *nMismatchesLUT = FingerprintsWhitebox::getNMismatchesLUT(fingerprints);
    
    // We check the mismatches for selected predefined results below.
    for (const auto &kv : map<size_t, size_t>({ { 0, 0 }, { 1, 1 }, { 2, 1 }, { 4, 1 }, { 5, 2 }, { 6, 2 }, { 7, 2 }, { 8, 1 } }))
    {
        REQUIRE(nMismatchesLUT[kv.first] == kv.second);
    }

    REQUIRE(nMismatchesLUT[0b000111010110] == 5);
    REQUIRE(nMismatchesLUT[0b000111111111] == 5);
    REQUIRE(nMismatchesLUT[0b000001010101] == 4);
}

TEST_CASE("is calculating words total size and counts correct for empty", "[fingerprints]")
{
    vector<string> words;
    size_t wordCountsBySize[maxWordSize + 1];

    size_t totalSize = FingerprintsWhitebox::calcTotalSize<FING_T>(words, wordCountsBySize);
    REQUIRE(totalSize == 0);

    for (size_t wordSize = 1; wordSize <= maxWordSize; ++wordSize)
    {
        REQUIRE(wordCountsBySize[wordSize] == 0);        
    }
}

TEST_CASE("is calculating words total size and counts correct", "[fingerprints]")
{
    vector<string> words { "ala", "ma", "kota", "a", "jarek", "ma", "psa" };
    size_t wordCountsBySize[maxWordSize + 1];

    size_t totalSize = FingerprintsWhitebox::calcTotalSize<FING_T>(words, wordCountsBySize);
    REQUIRE(totalSize == 34); // 20 chars + 7 * 2 bytes

    for (const auto &kv : map<size_t, size_t>({ { 1, 1 }, { 2, 2 }, { 3, 2 }, { 4, 1 }, { 5, 1 } }))
    {
        REQUIRE(wordCountsBySize[kv.first] == kv.second);        
    }

    for (size_t wordSize = 6; wordSize <= maxWordSize; ++wordSize)
    {
        REQUIRE(wordCountsBySize[wordSize] == 0);        
    }
}

TEST_CASE("is calculating words total size and counts correct for repeated strings", "[fingerprints]")
{
    for (int nStrings = 0; nStrings < maxNStrings; ++nStrings)
    {
        vector<string> words;
        
        for (int iStr = 0; iStr < nStrings; ++iStr)
        {
            words.emplace_back(string(stringSize, 'a'));
        }

        size_t wordCountsBySize[maxWordSize + 1];
        size_t totalSize = FingerprintsWhitebox::calcTotalSize<FING_T>(words, wordCountsBySize);

        REQUIRE(totalSize == nStrings * (sizeof(FING_T) + stringSize));

        for (size_t wordSize = 1; wordSize <= maxWordSize; ++wordSize)
        {
            if (wordSize != stringSize)
            {
                REQUIRE(wordCountsBySize[wordSize] == 0);
            }
        }

        REQUIRE(wordCountsBySize[stringSize] == nStrings);
    }
}

TEST_CASE("is calculating occurrence fingerprint for common letters correct", "[fingerprints]")
{
    // Passing fingerprint type 0 -- occurrence, letters type 0 - common (etaoinshrdlcumwf).
    Fingerprints<FING_T> fingerprints(0, 0);
    auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

    // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
    REQUIRE(fun("ala", 3) == 0b0000010000000100);
    REQUIRE(fun("instance", 8) == 0b0000100001110111);
    REQUIRE(fun("aaaaa", 5) == 0b0000000000000100);
    REQUIRE(fun("ebiz", 4) == 0b0000000000010001);
}

TEST_CASE("is calculating occurrence fingerprint for mixed letters correct", "[fingerprints]")
{
     // Passing fingerprint type 0 -- occurrence, letters type 1 - mixed (etaoinshzqxjkvbp).
    Fingerprints<FING_T> fingerprints(0, 1);
    auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

    REQUIRE(fun("ala", 3) == 0b0000000000000100);
    REQUIRE(fun("instance", 8) == 0b0000000001110111);
    REQUIRE(fun("aaaaa", 5) == 0b0000000000000100);
    REQUIRE(fun("ebiz", 4) == 0b0100000100010001);
}

TEST_CASE("is calculating occurrence fingerprint for rare letters correct", "[fingerprints]")
{
    // Passing fingerprint type 0 -- occurrence, letters type 2 - rare (zqxjkvbpygfwmucl).
    Fingerprints<FING_T> fingerprints(0, 2);
    auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

    REQUIRE(fun("ala", 3) == 0b1000000000000000);
    REQUIRE(fun("instance", 8) == 0b0000000000000000);
    REQUIRE(fun("aaaaa", 5) == 0b0000000000000000);
    REQUIRE(fun("ebiz", 4) == 0b0000000001000001);
}

TEST_CASE("is calculating count fingerprint for common letters correct", "[fingerprints]")
{
    // Passing fingerprint type 1 -- count, letters type 0 - common (etaoinsh).
    Fingerprints<FING_T> fingerprints(1, 0);
    auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

    // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
    REQUIRE(fun("ala", 3) == 0b0000000000100000);
    REQUIRE(fun("instance", 8) == 0b0001100100010101);
    REQUIRE(fun("aaaaa", 5) == 0b0000000000110000);
    REQUIRE(fun("ebiz", 4) == 0b0000000100000001);
}

TEST_CASE("is calculating count fingerprint for mixed letters correct", "[fingerprints]")
{
    // Passing fingerprint type 1 -- count, letters type 1 - mixed (etaokvbp).
    Fingerprints<FING_T> fingerprints(1, 1);
    auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

    // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
    REQUIRE(fun("ala", 3) == 0b0000000000100000);
    REQUIRE(fun("instance", 8) == 0b0000000000010101);
    REQUIRE(fun("aaaaa", 5) == 0b0000000000110000);
    REQUIRE(fun("ebiz", 4) == 0b0001000000000001);
}

TEST_CASE("is calculating count fingerprint for rare letters correct", "[fingerprints]")
{
    // Passing fingerprint type 1 -- count, letters type 2 - rare (zqxjkvbp).
    Fingerprints<FING_T> fingerprints(1, 2);
    auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

    // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
    REQUIRE(fun("ala", 3) == 0b0000000000000000);
    REQUIRE(fun("instance", 8) == 0b0000000000000000);
    REQUIRE(fun("aaaaa", 5) == 0b0000000000000000);
    REQUIRE(fun("ebiz", 4) == 0b0001000000000001);
}

TEST_CASE("is Hamming weight calculation correct", "[fingerprints]")
{
    // Hamming weight for 0 to 16.
    vector<unsigned int> res { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1 };

    for (unsigned int n = 0; n <= 16; ++n)
    {
        REQUIRE(FingerprintsWhitebox::calcHammingWeight<FING_T>(n) == res[n]);
    }
    
    REQUIRE(FingerprintsWhitebox::calcHammingWeight<FING_T>(0b000111010110) == 6);
    REQUIRE(FingerprintsWhitebox::calcHammingWeight<FING_T>(65535) == 16); // 2^16 - 1

}

TEST_CASE("is calculcating number of errors for occurrence fingerprint for common letters correct", "[fingerprints]")
{
    // Passing fingerprint type 0 -- occurrence, letters type 0 - common (etaoinshrdlcumwf).
    Fingerprints<FING_T> fingerprints(0, 0);
    auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

}

TEST_CASE("is calculcating number of errors for occurrence fingerprint for mixed letters correct", "[fingerprints]")
{
    // TODO
}

TEST_CASE("is calculcating number of errors for occurrence fingerprint for rare letters correct", "[fingerprints]")
{
    // TODO
}

TEST_CASE("is calculating number of errors for count fingerprint for common letters correct", "[fingerprints]")
{
    // TODO
}

TEST_CASE("is calculating number of errors for count fingerprint for mixed letters correct", "[fingerprints]")
{
    // TODO
}

TEST_CASE("is calculating number of errors for count fingerprint for rare letters correct", "[fingerprints]")
{
    // TODO
}

TEST_CASE("is Hamming at most k for empty calculation correct", "[fingerprints]")
{
    string empty = "";

    for (int k = 0; k < maxK; ++k)
    {
        REQUIRE(FingerprintsWhitebox::isHamAMK<FING_T>(empty.c_str(), empty.c_str(), 0, k) == true);    
    }
}

TEST_CASE("is Hamming at most k calculation for self correct", "[fingerprints]")
{
    string str1 = "ala ma kota";

    for (int k = 0; k < maxK; ++k)
    {
        REQUIRE(FingerprintsWhitebox::isHamAMK<FING_T>(str1.c_str(), str1.c_str(), str1.size(), k) == true);    
    }
}

TEST_CASE("is Hamming at most k=1 calculation correct", "[fingerprints]")
{
    string str = "ala ma kota";
    
    for (size_t i = 0; i < str.size(); ++i)
    {
        string cur = str;
        cur[i] = 'N';

        REQUIRE(FingerprintsWhitebox::isHamAMK<FING_T>(cur.c_str(), str.c_str(), cur.size(), 0) == false);
        REQUIRE(FingerprintsWhitebox::isHamAMK<FING_T>(cur.c_str(), str.c_str(), cur.size(), 1) == true);
    }
}

TEST_CASE("is Hamming at most k=1,2,3,4 randomized calculation correct", "[fingerprints]")
{
    string str = "ala ma kota";

    for (int k = 1; k <= 4; ++k)
    {
        repeat(nHammingRepeats, [str, k] {
            set<int> indexList = Helpers::randNumbersFromRange(0, str.size() - 1, k);
            string cur = str;

            for (const int i : indexList)
            {
                cur[i] = 'N';
            }

            for (int curK = 0; curK < k; ++curK)
            {
                REQUIRE(FingerprintsWhitebox::isHamAMK<FING_T>(cur.c_str(), str.c_str(), cur.size(), curK) == false);
            }
            for (int curK = k; curK <= 4; ++curK)
            {
                REQUIRE(FingerprintsWhitebox::isHamAMK<FING_T>(cur.c_str(), str.c_str(), cur.size(), curK) == true);
            }
        });
    };
}

} // namespace fingerprints
