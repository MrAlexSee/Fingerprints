#include <map>

#include "catch.hpp"

#include "fingerprints_whitebox.hpp"

#include "../fingerprints.hpp"
#include "../fingerprints.cpp"

using namespace std;

namespace fingerprints
{

namespace
{

constexpr int maxWordSize = 255;

constexpr int maxNStrings = 100;
constexpr int stringSize = 50;

constexpr int maxK = 10;

}

using FING_T = uint16_t;

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

TEST_CASE("is Hamming weight calculation correct", "[fingerprints]")
{
    // Hamming weight for 0 to 16.
    vector<unsigned int> res { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1 };

    for (unsigned int n = 0; n <= 16; ++n)
    {
        REQUIRE(FingerprintsWhitebox::calcHammingWeight<FING_T>(n) == res[n]);
    }

    // 2^16 - 1
    REQUIRE(FingerprintsWhitebox::calcHammingWeight<FING_T>(65535) == 16);
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

TEST_CASE("is Hamming at most k calculation correct", "[fingerprints]")
{
    // TODO
}

} // namespace fingerprints
