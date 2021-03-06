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

    for (int k = 0; k <= maxK; ++k)
    {
        REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(empty.c_str(), empty.c_str(), 0, k) == true);    
    }
}

TEST_CASE("is Hamming at most k calculation for self correct", "[distance]")
{
    string str1 = "ala ma kota";

    for (int k = 0; k <= maxK; ++k)
    {
        REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str1.c_str(), str1.c_str(), str1.size(), k) == true);    
    }
}

TEST_CASE("is Hamming at most k=1 calculation correct", "[distance]")
{
    string str = "ala ma kota";
    
    for (size_t i = 0; i < str.size(); ++i)
    {
        string cur = str;
        cur[i] = 'N';

        REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(cur.c_str(), str.c_str(), cur.size(), 0) == false);
        REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str.c_str(), cur.c_str(), cur.size(), 0) == false);

        REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(cur.c_str(), str.c_str(), cur.size(), 1) == true);
        REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str.c_str(), cur.c_str(), cur.size(), 1) == true);
    }
}

TEST_CASE("is Hamming at most k=1 calculation for selected words correct", "[distance]")
{
    string str1 = "aaaa", str2 = "aaab";
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str1.c_str(), str2.c_str(), str1.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str2.c_str(), str1.c_str(), str1.size(), 1) == true);

    str1 = "aaaa", str2 = "baab";
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str1.c_str(), str2.c_str(), str1.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str2.c_str(), str1.c_str(), str1.size(), 1) == false);

    str1 = "abcdef", str2 = "abccef";
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str1.c_str(), str2.c_str(), str1.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str2.c_str(), str1.c_str(), str1.size(), 1) == true);

    str1 = "abcdef", str2 = "abcccf";
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str1.c_str(), str2.c_str(), str1.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str2.c_str(), str1.c_str(), str1.size(), 1) == false);
}

TEST_CASE("is Hamming at most k=2 calculation for selected words correct", "[distance]")
{
    string str1 = "aaaa", str2 = "aabb";
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str1.c_str(), str2.c_str(), str1.size(), 2) == true);
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str2.c_str(), str1.c_str(), str1.size(), 2) == true);

    str1 = "aaaa", str2 = "abbb";
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str1.c_str(), str2.c_str(), str1.size(), 2) == false);
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str2.c_str(), str1.c_str(), str1.size(), 2) == false);

    str1 = "abcdef", str2 = "abcccf";
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str1.c_str(), str2.c_str(), str1.size(), 2) == true);
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str2.c_str(), str1.c_str(), str1.size(), 2) == true);

    str1 = "abcdef", str2 = "abcccc";
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str1.c_str(), str2.c_str(), str1.size(), 2) == false);
    REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str2.c_str(), str1.c_str(), str1.size(), 2) == false);
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
                REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(cur.c_str(), str.c_str(), cur.size(), curK) == false);
                REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str.c_str(), cur.c_str(), cur.size(), curK) == false);
            }
            for (int curK = k; curK <= 4; ++curK)
            {
                REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(cur.c_str(), str.c_str(), cur.size(), curK) == true);
                REQUIRE(FingerprintsWhitebox::isHamAtMostK<FING_T>(str.c_str(), cur.c_str(), cur.size(), curK) == true);
            }
        });
    };
}

TEST_CASE("is Leven at most k for empty calculation correct", "[distance]")
{
    // Passing distance type Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(Fingerprints<FING_T>::DistanceType::Lev, 
        Fingerprints<FING_T>::FingerprintType::Occ, Fingerprints<FING_T>::LettersType::Common);
    string empty = "";

    for (int k = 0; k <= maxK; ++k)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, empty.c_str(), empty.size(), empty.c_str(), empty.size(), k) == true);
    }
}

TEST_CASE("is Leven at most k calculation for self correct", "[distance]")
{
    // Passing distance type Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(Fingerprints<FING_T>::DistanceType::Lev, 
        Fingerprints<FING_T>::FingerprintType::Occ, Fingerprints<FING_T>::LettersType::Common);
    string str1 = "ala ma kota";

    for (int k = 0; k <= maxK; ++k)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str1.c_str(), str1.size(), k) == true);
    }
}

TEST_CASE("is Leven at most k=0 calculation for selected words correct", "[distance]")
{
    // Passing distance type Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(Fingerprints<FING_T>::DistanceType::Lev, 
        Fingerprints<FING_T>::FingerprintType::Occ, Fingerprints<FING_T>::LettersType::Common);
    string str1 = "ala", str2 = "kota";

    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str1.c_str(), str1.size(), 0) == true);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str2.c_str(), str2.size(), 0) == true);

    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 0) == false);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 0) == false);
}

TEST_CASE("is Leven at most k=1 calculation correct", "[distance]")
{
    // Passing distance type Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(Fingerprints<FING_T>::DistanceType::Lev, 
        Fingerprints<FING_T>::FingerprintType::Occ, Fingerprints<FING_T>::LettersType::Common);
    const string str = "ala ma kota";

    // Substitutions.
    for (size_t i = 0; i < str.size(); ++i)
    {
        string cur = str;
        cur[i] = 'N';

        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, cur.c_str(), cur.size(), str.c_str(), str.size(), 0) == false);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), cur.c_str(), cur.size(), 0) == false);

        for (int k = 1; k <= maxK; ++k)
        {
            REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, cur.c_str(), cur.size(), str.c_str(), str.size(), k) == true);
            REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), cur.c_str(), cur.size(), k) == true);
        }
    }

    // Insertions.
    for (size_t i = 0; i < str.size(); ++i)
    {
        string cur = str.substr(0, i) + "N" + str.substr(i, string::npos);

        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, cur.c_str(), cur.size(), str.c_str(), str.size(), 0) == false);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), cur.c_str(), cur.size(), 0) == false);

        for (int k = 1; k <= maxK; ++k)
        {
            REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, cur.c_str(), cur.size(), str.c_str(), str.size(), k) == true);
            REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), cur.c_str(), cur.size(), k) == true);
        }
    }

    // Deletions.
    for (size_t i = 0; i < str.size(); ++i)
    {
        string cur = str.substr(0, i) + str.substr(i + 1, string::npos);

        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, cur.c_str(), cur.size(), str.c_str(), str.size(), 0) == false);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), cur.c_str(), cur.size(), 0) == false);

        for (int k = 1; k <= maxK; ++k)
        {
            REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, cur.c_str(), cur.size(), str.c_str(), str.size(), k) == true);
            REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), cur.c_str(), cur.size(), k) == true);
        }
    }
}

TEST_CASE("is Leven at most k=1 calculation for selected words correct", "[distance]")
{
    // Passing distance type Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(Fingerprints<FING_T>::DistanceType::Lev, 
        Fingerprints<FING_T>::FingerprintType::Occ, Fingerprints<FING_T>::LettersType::Common);

    string str1 = "aaaa", str2 = "aaab";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == true);

    str1 = "aaaa", str2 = "baab";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);

    str1 = "ala", str2 = "alak";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == true);

    str1 = "ala", str2 = "kota";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);

    str1 = "ala", str2 = "al";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == true);

    str1 = "ala", str2 = "ma";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);

    str1 = "ala", str2 = "aa";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == true);

    str1 = "jarek", str2 = "jaek";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == true);

    str1 = "jarek", str2 = "jarrek";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == true);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == true);

    str1 = "jarek", str2 = "jak";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);

    str1 = "jarek", str2 = "jarrrek";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);
}

TEST_CASE("is Leven at most k=1 calculation after match for selected words correct", "[distance]")
{
    // Passing distance type Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(Fingerprints<FING_T>::DistanceType::Lev, 
        Fingerprints<FING_T>::FingerprintType::Occ, Fingerprints<FING_T>::LettersType::Common);
   
    string str1 = "ala", str2 = "kota"; 
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str1.c_str(), str1.size(), 1) == true);

    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);

    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str2.c_str(), str2.size(), 1) == true);

    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 1) == false);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 1) == false);
}

TEST_CASE("is Leven at most k=1 calculation for various deletions correct", "[distance]")
{
    // Passing distance type Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(Fingerprints<FING_T>::DistanceType::Lev, 
        Fingerprints<FING_T>::FingerprintType::Occ, Fingerprints<FING_T>::LettersType::Common);
    
    string str = "jarek";
    
    vector<string> inStrings { "arek", "jrek", "jaek", "jark", "jare" };
    vector<string> outStrings { "ja", "jr", "je", "jk", "raj", "eaj", "erj", "jjk", "jrr", "jar", "jae", "jak", "jre", "jrk", "jek", "aaek", "jakk", "jajk", "jjre" };
    
    for (const string &inStr : inStrings)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), inStr.c_str(), inStr.size(), 1) == true);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, inStr.c_str(), inStr.size(), str.c_str(), str.size(), 1) == true);
    }

    for (const string &outStr : outStrings)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), outStr.c_str(), outStr.size(), 1) == false);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, outStr.c_str(), outStr.size(), str.c_str(), str.size(), 1) == false);
    }
}

TEST_CASE("is Leven at most k=1 calculation for various insertions correct", "[distance]")
{
    // Passing distance type Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(Fingerprints<FING_T>::DistanceType::Lev, 
        Fingerprints<FING_T>::FingerprintType::Occ, Fingerprints<FING_T>::LettersType::Common);
    
    string str = "jarek";
    
    vector<string> inStrings { "jarekk", "jareek", "jarrek", "jaarek", "jjarek", "jNarek", "jaNrek", "jarNek", "jareNk", "jarekN" };
    vector<string> outStrings { "jjaarek", "jjarrek", "jjareek", "jjarekk" };
    
    for (const string &inStr : inStrings)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), inStr.c_str(), inStr.size(), 1) == true);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, inStr.c_str(), inStr.size(), str.c_str(), str.size(), 1) == true);
    }

    for (const string &outStr : outStrings)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), outStr.c_str(), outStr.size(), 1) == false);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, outStr.c_str(), outStr.size(), str.c_str(), str.size(), 1) == false);
    }
}

TEST_CASE("is Leven at most k=1 calculation for various substitutions correct", "[distance]")
{
    // Passing distance type Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(Fingerprints<FING_T>::DistanceType::Lev, 
        Fingerprints<FING_T>::FingerprintType::Occ, Fingerprints<FING_T>::LettersType::Common);

    string str = "jarek";

    vector<string> inStrings { "jacek", "japek", "karek", "darek", "sarek" };
    vector<string> outStrings { "gacek", "pacek", "tadek", "radek" };
    
    for (const string &inStr : inStrings)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), inStr.c_str(), inStr.size(), 1) == true);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, inStr.c_str(), inStr.size(), str.c_str(), str.size(), 1) == true);
    }

    for (const string &outStr : outStrings)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), outStr.c_str(), outStr.size(), 1) == false);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, outStr.c_str(), outStr.size(), str.c_str(), str.size(), 1) == false);
    }
}

TEST_CASE("is Leven at most k=2 calculation for selected words correct", "[distance]")
{
    // Passing distance type Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(Fingerprints<FING_T>::DistanceType::Lev, 
        Fingerprints<FING_T>::FingerprintType::Occ, Fingerprints<FING_T>::LettersType::Common);

    string str1 = "aaaa", str2 = "aabb";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 2) == true);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 2) == true);

    str1 = "aaaa", str2 = "aa";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 2) == true);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 2) == true);

    str1 = "aaaa", str2 = "aaaaaa";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 2) == true);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 2) == true);

    str1 = "aaaa", str2 = "aaccaa";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 2) == true);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 2) == true);

    str1 = "aaaa", str2 = "a";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 2) == false);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 2) == false);

    str1 = "aaaa", str2 = "aaaaaaa";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 2) == false);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 2) == false);

    str1 = "aaaa", str2 = "accc";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 2) == false);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 2) == false);

    str1 = "aaaa", str2 = "aaaaccc";
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str1.c_str(), str1.size(), str2.c_str(), str2.size(), 2) == false);
    REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str2.c_str(), str2.size(), str1.c_str(), str1.size(), 2) == false);
}

TEST_CASE("is Leven at most k=2 calculation for various deletions correct", "[distance]")
{
    // Passing distance type Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(Fingerprints<FING_T>::DistanceType::Lev, 
        Fingerprints<FING_T>::FingerprintType::Occ, Fingerprints<FING_T>::LettersType::Common);
    
    string str = "jarek";
    
    vector<string> inStrings { "jar", "jae", "jak", "jre", "jrk", "jek", "arek", "jrek", "jaek", "jark", "jare" };
    vector<string> outStrings { "ja", "jr", "je", "jk", "raj", "eaj", "erj", "jjk", "jrr" };
    
    for (const string &inStr : inStrings)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), inStr.c_str(), inStr.size(), 2) == true);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, inStr.c_str(), inStr.size(), str.c_str(), str.size(), 2) == true);
    }

    for (const string &outStr : outStrings)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), outStr.c_str(), outStr.size(), 2) == false);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, outStr.c_str(), outStr.size(), str.c_str(), str.size(), 2) == false);
    }
}

TEST_CASE("is Leven at most k=2 calculation for various insertions correct", "[distance]")
{
    // Passing distance type Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(Fingerprints<FING_T>::DistanceType::Lev, 
        Fingerprints<FING_T>::FingerprintType::Occ, Fingerprints<FING_T>::LettersType::Common);
    
    string str = "jarek";
    
    vector<string> inStrings { "jarekk", "jarekkk", "jarrek", "jarrrek", "jaareek", "jjarekk" };
    vector<string> outStrings { "jarekkkk", "jarrrrek", "jaaareek", "jaareeek", "jaaareeek" };
    
    for (const string &inStr : inStrings)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), inStr.c_str(), inStr.size(), 2) == true);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, inStr.c_str(), inStr.size(), str.c_str(), str.size(), 2) == true);
    }

    for (const string &outStr : outStrings)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), outStr.c_str(), outStr.size(), 2) == false);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, outStr.c_str(), outStr.size(), str.c_str(), str.size(), 2) == false);
    }
}

TEST_CASE("is Leven at most k=2 calculation for various modifications correct", "[distance]")
{
    // Passing distance type Levenshtein, fingerprint and letter types don't matter here.
    Fingerprints<FING_T> fingerprints(Fingerprints<FING_T>::DistanceType::Lev, 
        Fingerprints<FING_T>::FingerprintType::Occ, Fingerprints<FING_T>::LettersType::Common);
    
    string str = "jarek";
    
    vector<string> inStrings { "jaark", "jrekk", "arekk", "aree" };
    vector<string> outStrings { "jjjjj", "jjjj", "jjj", "jrko" };
    
    for (const string &inStr : inStrings)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), inStr.c_str(), inStr.size(), 2) == true);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, inStr.c_str(), inStr.size(), str.c_str(), str.size(), 2) == true);
    }

    for (const string &outStr : outStrings)
    {
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, str.c_str(), str.size(), outStr.c_str(), outStr.size(), 2) == false);
        REQUIRE(FingerprintsWhitebox::isLevAtMostK<FING_T>(fingerprints, outStr.c_str(), outStr.size(), str.c_str(), str.size(), 2) == false);
    }
}

} // namespace fingerprints
