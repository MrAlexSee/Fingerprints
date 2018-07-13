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

constexpr int maxK = 5;
constexpr int nHammingRepeats = 10;

using FING_T = uint16_t;

}

TEST_CASE("is Hamming at most k for empty calculation correct", "[distance]")
{
    string empty = "";

    for (int k = 0; k < maxK; ++k)
    {
        REQUIRE(FingerprintsWhitebox::isHamAMK<FING_T>(empty.c_str(), empty.c_str(), 0, k) == true);    
    }
}

TEST_CASE("is Hamming at most k calculation for self correct", "[distance]")
{
    string str1 = "ala ma kota";

    for (int k = 0; k < maxK; ++k)
    {
        REQUIRE(FingerprintsWhitebox::isHamAMK<FING_T>(str1.c_str(), str1.c_str(), str1.size(), k) == true);    
    }
}

TEST_CASE("is Hamming at most k=1 calculation correct", "[distance]")
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

TEST_CASE("is Hamming at most k=1 calculation selected words correct", "[distance]")
{
    string str1 = "aaaa", str2 = "aaab";
    
    REQUIRE(FingerprintsWhitebox::isHamAMK<FING_T>(str1.c_str(), str2.c_str(), str1.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isHamAMK<FING_T>(str2.c_str(), str1.c_str(), str1.size(), 1) == true);

    str1 = "aaaa", str2 = "baab";
    REQUIRE(FingerprintsWhitebox::isHamAMK<FING_T>(str1.c_str(), str2.c_str(), str1.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isHamAMK<FING_T>(str2.c_str(), str1.c_str(), str1.size(), 1) == false);
}

TEST_CASE("is Hamming at most k=1,2,3,4 randomized calculation correct", "[distance]")
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

TEST_CASE("is Leven at most k=0 calculation for selected words correct", "[distance]")
{
    // Passing distance type 1 -- Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(1, 0, 0);
    string str1 = "ala", str2 = "kota";

    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str1.c_str(), str1.size(), 0) == true);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str2.c_str(), str2.size(), 0) == true);

    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 0) == false);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 0) == false);
}

TEST_CASE("is Leven at most k=1 calculation correct", "[distance]")
{
    // Passing distance type 1 -- Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(1, 0, 0);

    string str = "ala ma kota";

    for (size_t i = 0; i < str.size(); ++i)
    {
        string cur = str;
        cur[i] = 'N';

        REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, cur.c_str(), cur.size(), str.c_str(), str.size(), 0) == false);
        REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, cur.c_str(), cur.size(), str.c_str(), str.size(), 1) == true);
    }
}

TEST_CASE("is Leven at most k=1 calculation selected words correct", "[distance]")
{
    // Passing distance type 1 -- Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(1, 0, 0);

    string str1 = "aaaa", str2 = "aaab";
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == true);

    str1 = "aaaa", str2 = "baab";
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);

    str1 = "ala", str2 = "alak";
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == true);

    str1 = "ala", str2 = "kota";
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);

    str1 = "ala", str2 = "al";
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == true);

    str1 = "ala", str2 = "ma";
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);

    str1 = "ala", str2 = "aa";
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == true);

    str1 = "jarek", str2 = "jaek";
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == true);

    str1 = "jarek", str2 = "jarrek";
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == true);

    str1 = "jarek", str2 = "jak";
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);

    str1 = "jarek", str2 = "jarrrek";
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);
}

TEST_CASE("is Leven at most k=1 calculation after match for selected words correct", "[distance]")
{
    // Passing distance type 1 -- Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(1, 0, 0);
    string str1 = "ala", str2 = "kota";
    
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str1.c_str(), str1.size(), 1) == true);

    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);

    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str2.c_str(), str2.size(), 1) == true);

    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAMK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);
}

} // namespace fingerprints
