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

constexpr int maxNStrings = 50;
constexpr int stringSize = 25;

vector<int> distanceTypes { 0, 1 };
vector<int> fingerprintTypes { -1, 0, 1, 2, 3 };
vector<int> lettersTypes { 0, 1, 2 };

constexpr int maxK = 5;

using FING_T = uint16_t;

}

TEST_CASE("is initializing using fingerprints correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        for (int lettersType : lettersTypes)
        {
            Fingerprints<FING_T> fno(distanceType, -1, lettersType);
            REQUIRE(FingerprintsWhitebox::getUseFingerprints(fno) == false);

            for (int fingerprintType : { 0, 1, 2 })
            {
                Fingerprints<FING_T> f(distanceType, fingerprintType, lettersType);
                REQUIRE(FingerprintsWhitebox::getUseFingerprints(f) == true);
            }
        }
    }
}

TEST_CASE("is initializing using Hamming correct", "[fingerprints]")
{
    for (int fingerprintType : fingerprintTypes)
    {
        for (int lettersType : lettersTypes)
        {
            Fingerprints<FING_T> fham(0, fingerprintType, lettersType);
            REQUIRE(FingerprintsWhitebox::getUseHamming(fham) == true);

            Fingerprints<FING_T> flev(1, fingerprintType, lettersType);
            REQUIRE(FingerprintsWhitebox::getUseHamming(flev) == false);
        }
    }
}

TEST_CASE("is searching empty words correct", "[fingerprints]")
{
    vector<string> words;
    vector<string> patterns { "ala", "ma", "kota", "a", "jarek", "ma", "psa" };

    for (int distanceType : distanceTypes)
    {
        for (int fingerprintType : fingerprintTypes)
        {
            for (int lettersType : lettersTypes)
            {
                Fingerprints<FING_T> curF(distanceType, fingerprintType, lettersType);
                curF.preprocess(words);

                for (int k = 0; k <= maxK; ++k)
                {
                    REQUIRE(curF.test(patterns, k) == 0);
                }
            }
        }
    }
}

TEST_CASE("is searching words exact correct", "[fingerprints]")
{
    vector<string> words { "ala", "ma", "kota", "a", "jarek", "ma", "psa", "ma" };
    vector<string> patternsOut { "not", "in", "this", "dict" };

    for (int distanceType : distanceTypes)
    {
        for (int fingerprintType : fingerprintTypes)
        {
            for (int lettersType : lettersTypes)
            {
                Fingerprints<FING_T> curF(distanceType, fingerprintType, lettersType);
                curF.preprocess(words);

                REQUIRE(curF.test(words, 0) == words.size());
                REQUIRE(curF.test(patternsOut, 0) == 0);
            }
        }
    }
}

TEST_CASE("is searching words exact one-by-one correct", "[fingerprints]")
{
    vector<string> words { "ala", "ma", "kota", "a", "jarek", "ma", "psa", "ma" };
    vector<string> patternsOut { "not", "in", "this", "dict" };

    for (int distanceType : distanceTypes)
    {
        for (int fingerprintType : fingerprintTypes)
        {
            for (int lettersType : lettersTypes)
            {
                Fingerprints<FING_T> curF(distanceType, fingerprintType, lettersType);
                curF.preprocess(words);

                for (const string &word : words)
                {
                    REQUIRE(curF.test(vector<string> { word }, 0) == 1);
                }

                for (const string &patternOut : patternsOut)
                {
                    REQUIRE(curF.test(vector<string> { patternOut }, 0) == 0);
                }
            }
        }
    }
}

TEST_CASE("is searching words exact randomized correct", "[fingerprints]")
{
    for (int nStrings = 1; nStrings <= maxNStrings; ++nStrings)
    {
        vector<string> words;
        repeat(nStrings, [&words] {
            words.emplace_back(Helpers::genRandomStringAlphNum(stringSize));
        });

        // Use different sizes than stringSize here.
        vector<string> patternsOut { "not", "in", "this", "dict" };

        for (int distanceType : distanceTypes)
        {
            for (int fingerprintType : fingerprintTypes)
            {
                for (int lettersType : lettersTypes)
                {   
                    Fingerprints<FING_T> curF(distanceType, fingerprintType, lettersType);
                    curF.preprocess(words);

                    REQUIRE(curF.test(words, 0) == words.size());
                    REQUIRE(curF.test(patternsOut, 0) == 0);
                }
            }
        }
    }
}

TEST_CASE("is searching words for k = 1 for Hamming correct", "[fingerprints]")
{
    vector<string> words { "ala", "ma", "kota", "a", "jarek", "ma", "psa" };
    
    vector<string> patternsOut { "not", "in", "this", "dict" };
    vector<string> patternsIn;

    for (const string &word : words)
    {
        for (size_t i = 0; i < word.size(); ++i)
        {
            string curWord = word;
            curWord[i] = 'N';

            patternsIn.emplace_back(move(curWord));
        }
    }

    for (int fingerprintType : fingerprintTypes)
    {
        for (int lettersType : lettersTypes)
        {
            Fingerprints<FING_T> curF(0, fingerprintType, lettersType);
            curF.preprocess(words);

            REQUIRE(curF.test(patternsIn, 1) == patternsIn.size());
            REQUIRE(curF.test(patternsOut, 1) == 0);
        }
    }
}

TEST_CASE("is searching words for k = 1 for Hamming one-by-one correct", "[fingerprints]")
{
    vector<string> words { "ala", "ma", "kota", "a", "jarek", "ma", "psa" };
    
    vector<string> patternsOut { "not", "in", "this", "dict" };
    vector<string> patternsIn;

    for (const string &word : words)
    {
        for (size_t i = 0; i < word.size(); ++i)
        {
            string curWord = word;
            curWord[i] = 'N';

            patternsIn.emplace_back(move(curWord));
        }
    }

    for (int fingerprintType : fingerprintTypes)
    {
        for (int lettersType : lettersTypes)
        {
            Fingerprints<FING_T> curF(0, fingerprintType, lettersType);
            curF.preprocess(words);

            for (const string &patternIn : patternsIn)
            {
                REQUIRE(curF.test(vector<string> { patternIn }, 1) == 1);
            }

            for (const string &patternOut : patternsOut)
            {
                REQUIRE(curF.test(vector<string> { patternOut }, 1) == 0);
            }
        }
    }
}

TEST_CASE("is searching words for various k for Hamming randomized correct", "[fingerprints]")
{
    for (int k = 1; k <= maxK; ++k)
    {
        REQUIRE(k <= stringSize);

        for (int nStrings = 1; nStrings <= maxNStrings; ++nStrings)
        {
            vector<string> words;
            repeat(nStrings, [&words] {
                words.emplace_back(Helpers::genRandomStringAlphNum(stringSize));
            });

            // Use different sizes than stringSize here.
            vector<string> patternsOut { "not", "in", "this", "dict" };
            vector<string> patternsIn;

            for (const string &word : words)
            {
                string curWord = word;
                set<int> indexList = Helpers::randNumbersFromRange(0, word.size() - 1, k);

                for (int index : indexList)
                {
                    curWord[index] = 'N';
                }

                patternsIn.emplace_back(move(curWord));
            }

            for (int fingerprintType : fingerprintTypes)
            {
                for (int lettersType : lettersTypes)
                {   
                    Fingerprints<FING_T> curF(0, fingerprintType, lettersType);
                    curF.preprocess(words);

                    // Note: there might be more matches due to randomization, but at least one word
                    // must have at least a single match.
                    REQUIRE(curF.test(patternsIn, k) >= words.size());
                    REQUIRE(curF.test(patternsOut, k) == 0);
                }
            }
        }
    }
}

TEST_CASE("is searching words for k = 1 for Levenshtein correct", "[fingerprints]")
{
    vector<string> words { "ala", "ma", "kota", "a", "jarek", "ma", "psa" };
    
    vector<string> patternsIn { "bla", "alak", "alla", "darek", "jaek", "jarrek", "ps" };
    vector<string> patternsOut { "not", "in", "this", "dict" };

    for (int fingerprintType : fingerprintTypes)
    {
        for (int lettersType : lettersTypes)
        {
            Fingerprints<FING_T> curF(1, fingerprintType, lettersType);
            curF.preprocess(words);

            REQUIRE(curF.test(patternsIn, 1) == patternsIn.size());
            REQUIRE(curF.test(patternsOut, 1) == 0);
        }
    }
}

TEST_CASE("is searching words for k = 1 for Levenshtein one-by-one correct", "[fingerprints]")
{
    vector<string> words { "ala", "ma", "kota", "a", "jarek", "ma", "psa" };
    
    vector<string> patternsIn { "bla", "alak", "alla", "darek", "jaek", "jarrek", "ps" };
    vector<string> patternsOut { "not", "in", "this", "dict" };

    for (int fingerprintType : fingerprintTypes)
    {
        for (int lettersType : lettersTypes)
        {
            Fingerprints<FING_T> curF(1, fingerprintType, lettersType);
            curF.preprocess(words);

            for (const string &patternIn : patternsIn)
            {
                REQUIRE(curF.test(vector<string> { patternIn }, 1) == 1);
            }

            for (const string &patternOut : patternsOut)
            {
                REQUIRE(curF.test(vector<string> { patternOut }, 1) == 0);
            }
        }
    }
}

TEST_CASE("is searching words for k = 2 for Levenshtein correct", "[fingerprints]")
{
    vector<string> words { "ala", "kota", "jarek", "psa", "pies", "stolik" };
    
    vector<string> patternsIn { "blb", "kota", "alla", "allla", "jak", "jarrrek", "stok", "stol" };
    vector<string> patternsOut { "isnot", "in", "this", "dict" };

    for (int fingerprintType : fingerprintTypes)
    {
        for (int lettersType : lettersTypes)
        {
            Fingerprints<FING_T> curF(1, fingerprintType, lettersType);
            curF.preprocess(words);

            REQUIRE(curF.test(patternsIn, 2) == patternsIn.size());
            REQUIRE(curF.test(patternsOut, 2) == 0);
        }
    }
}

TEST_CASE("is searching words for k = 2 for Levenshtein one-by-one correct", "[fingerprints]")
{
    vector<string> words { "ala", "kota", "jarek", "psa", "pies", "stolik" };
    
    vector<string> patternsIn { "blb", "kota", "alla", "allla", "jak", "jarrrek", "stok", "stol" };
    vector<string> patternsOut { "isnot", "in", "this", "dict" };

    for (int fingerprintType : fingerprintTypes)
    {
        for (int lettersType : lettersTypes)
        {
            Fingerprints<FING_T> curF(1, fingerprintType, lettersType);
            curF.preprocess(words);

            for (const string &patternIn : patternsIn)
            {
                REQUIRE(curF.test(vector<string> { patternIn }, 2) == 1);
            }

            for (const string &patternOut : patternsOut)
            {
                REQUIRE(curF.test(vector<string> { patternOut }, 2) == 0);
            }
        }
    }
}

TEST_CASE("is calculating rejection for k = 1 for occurrence common fingerprints correct", "[fingerprints]")
{
    vector<string> words { "kotaa", "jacek", "piesy" };
    
    vector<string> patternsOut { "zzzzz", "qzxyu" };
    vector<string> patternsMixed { "zzzzz", "kotaa" };

    // Distance type shouldn't matter for rejection calculation.
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 0 -- occurrence, letters type 0 - common (etaoinshrdlcumwf).
        Fingerprints<FING_T> fingerprints(distanceType, 0, 0);
        fingerprints.preprocess(words);

        REQUIRE(fingerprints.testRejection(patternsOut, 1) == 1.0f);
        REQUIRE(fingerprints.testRejection(patternsMixed, 1) == Approx(0.8333333f));
    }
}

TEST_CASE("is calculating rejection for k = 1 for occurrence mixed fingerprints correct", "[fingerprints]")
{
    vector<string> words { "kotaa", "jacek", "piesy" };
    
    vector<string> patternsOut { "zzzzz", "mcuyu" };
    vector<string> patternsMixed { "zzzzz", "kotaa" };

    // Distance type shouldn't matter for rejection calculation.
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 0 -- occurrence, letters type 1 - mixed (etaoinshzqxjkvbp).
        Fingerprints<FING_T> fingerprints(distanceType, 0, 1);
        fingerprints.preprocess(words);

        REQUIRE(fingerprints.testRejection(patternsOut, 1) == 1.0f);
        REQUIRE(fingerprints.testRejection(patternsMixed, 1) == Approx(0.8333333f));
    }
}

TEST_CASE("is calculating rejection for k = 1 for occurrence rare fingerprints correct", "[fingerprints]")
{
    vector<string> words { "kbpyy", "jacek", "piecy" };
    
    vector<string> patternsOut { "zzzzz", "etaoi" };
    vector<string> patternsMixed { "zzzzz", "jarek" };

    // Distance type shouldn't matter for rejection calculation.
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 0 -- occurrence, letters type 2 - rare (zqxjkvbpygfwmucl).
        Fingerprints<FING_T> fingerprints(distanceType, 0, 2);
        fingerprints.preprocess(words);

        REQUIRE(fingerprints.testRejection(patternsOut, 1) == 1.0f);
        REQUIRE(fingerprints.testRejection(patternsMixed, 1) == Approx(0.8333333f));
    }
}

TEST_CASE("is initializing errors LUT correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        for (int fingerprintType : fingerprintTypes)
        {
            for (int lettersType : lettersTypes)
            {
                Fingerprints<FING_T> fingerprints(distanceType, fingerprintType, lettersType);

                const unsigned char *nErrorsLUT = FingerprintsWhitebox::getNErrorsLUT(fingerprints);

                REQUIRE(nErrorsLUT[0] == 0);
                REQUIRE(nErrorsLUT[1] == 1);
                REQUIRE(nErrorsLUT[2] == 1);
                REQUIRE(nErrorsLUT[3] == 2);
                REQUIRE(nErrorsLUT[4] == 2);
                REQUIRE(nErrorsLUT[16] == 8);
            }
        }
    }
}

TEST_CASE("is initializing chars map for 16 common letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // This is checked for an occurrence fingerprint.
        Fingerprints<FING_T> fingerprints(distanceType, 0, 0);

        const unsigned char *charsMap = FingerprintsWhitebox::getCharsMap(fingerprints);
        string letters = "etaoinshrdlcumwf";

        for (size_t i = 0; i < letters.size(); ++i)
        {
            REQUIRE(charsMap[static_cast<size_t>(letters[i])] == i);
        }
    }
}

TEST_CASE("is initializing chars map for 8 common letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // This is checked for a count fingerprint.
        Fingerprints<FING_T> fingerprints(distanceType, 1, 0);

        const unsigned char *charsMap = FingerprintsWhitebox::getCharsMap(fingerprints);
        string letters = "etaoinsh";

        for (size_t i = 0; i < letters.size(); ++i)
        {
            REQUIRE(charsMap[static_cast<size_t>(letters[i])] == i);
        }
    }
}

TEST_CASE("is initializing char list for 5 common letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // This is checked for a position fingerprint.
        Fingerprints<FING_T> fingerprints(distanceType, 2, 0);

        const unsigned char *charList = FingerprintsWhitebox::getCharList(fingerprints);
        string letters = "etaoin";

        for (size_t i = 0; i < letters.size(); ++i)
        {
            REQUIRE(charList[i] == letters[i]);
        }
    }
}

TEST_CASE("is calculating mismatches LUT for occurrence fingerprints correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        for (int lettersType : lettersTypes)
        {
            Fingerprints<FING_T> fingerprints(distanceType, 0, lettersType);
            const unsigned char *nMismatchesLUT = FingerprintsWhitebox::getNMismatchesLUT(fingerprints);
            
            // We check the mismatches for selected predefined results below.
            for (const auto &kv : map<size_t, size_t>({ { 0, 0 }, { 1, 1 }, { 2, 1 }, { 4, 1 }, { 5, 2 }, { 6, 2 }, { 7, 3 }, { 8, 1 } }))
            {
                REQUIRE(nMismatchesLUT[kv.first] == kv.second);
            }

            REQUIRE(nMismatchesLUT[0b000001010101] == 4);
            REQUIRE(nMismatchesLUT[0b000111010110] == 6);
            REQUIRE(nMismatchesLUT[0b000111010110] == 6);
            REQUIRE(nMismatchesLUT[0b000111111111] == 9);
        }
    }
}

TEST_CASE("is calculating mismatches LUT for count fingerprints correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        for (int lettersType : lettersTypes)
        {
            Fingerprints<FING_T> fingerprints(distanceType, 1, lettersType);
            const unsigned char *nMismatchesLUT = FingerprintsWhitebox::getNMismatchesLUT(fingerprints);
            
            // We check the mismatches for selected predefined results below.
            for (const auto &kv : map<size_t, size_t>({ { 0, 0 }, { 1, 1 }, { 2, 1 }, { 4, 1 }, { 5, 2 }, { 6, 2 }, { 7, 2 }, { 8, 1 } }))
            {
                REQUIRE(nMismatchesLUT[kv.first] == kv.second);
            }

            REQUIRE(nMismatchesLUT[0b000001010101] == 4);
            REQUIRE(nMismatchesLUT[0b000111010110] == 5);
            REQUIRE(nMismatchesLUT[0b000111010110] == 5);
            REQUIRE(nMismatchesLUT[0b000111111111] == 5);
        }
    }
}

TEST_CASE("is calculating mismatches LUT for position fingerprints correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        for (int lettersType : lettersTypes)
        {
            Fingerprints<FING_T> fingerprints(distanceType, 2, lettersType);
            const unsigned char *nMismatchesLUT = FingerprintsWhitebox::getNMismatchesLUT(fingerprints);

            // We check the mismatches for selected predefined results below.
            for (const auto &kv : map<size_t, size_t>({ { 0, 0 }, { 1, 1 }, { 2, 1 }, { 4, 1 }, { 5, 1 }, { 6, 1 }, { 7, 1 }, { 8, 1 } }))
            {
                REQUIRE(nMismatchesLUT[kv.first] == kv.second);
            }

            REQUIRE(nMismatchesLUT[0b000001010101] == 3);
            REQUIRE(nMismatchesLUT[0b100001010101] == 4);
            REQUIRE(nMismatchesLUT[0b000111010110] == 3);
            REQUIRE(nMismatchesLUT[0b000111010110] == 3);
            REQUIRE(nMismatchesLUT[0b000111111111] == 3);
        }
    }
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
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 0 -- occurrence, letters type 0 - common (etaoinshrdlcumwf).
        Fingerprints<FING_T> fingerprints(distanceType, 0, 0);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
        REQUIRE(fun("ala", 3) == 0b0000010000000100);
        REQUIRE(fun("instance", 8) == 0b0000100001110111);
        REQUIRE(fun("aaaaa", 5) == 0b0000000000000100);
        REQUIRE(fun("ebiz", 4) == 0b0000000000010001);
    }
}

TEST_CASE("is calculating occurrence fingerprint for mixed letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 0 -- occurrence, letters type 1 - mixed (etaoinshzqxjkvbp).
        Fingerprints<FING_T> fingerprints(distanceType, 0, 1);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        REQUIRE(fun("ala", 3) == 0b0000000000000100);
        REQUIRE(fun("instance", 8) == 0b0000000001110111);
        REQUIRE(fun("aaaaa", 5) == 0b0000000000000100);
        REQUIRE(fun("ebiz", 4) == 0b0100000100010001);
    }
}

TEST_CASE("is calculating occurrence fingerprint for rare letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 0 -- occurrence, letters type 2 - rare (zqxjkvbpygfwmucl).
        Fingerprints<FING_T> fingerprints(distanceType, 0, 2);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        REQUIRE(fun("ala", 3) == 0b1000000000000000);
        REQUIRE(fun("instance", 8) == 0b0100000000000000);
        REQUIRE(fun("aaaaa", 5) == 0b0000000000000000);
        REQUIRE(fun("ebiz", 4) == 0b0000000001000001);
    }
}

TEST_CASE("is calculating count fingerprint for common letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 1 -- count, letters type 0 - common (etaoinsh).
        Fingerprints<FING_T> fingerprints(distanceType, 1, 0);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
        REQUIRE(fun("ala", 3) == 0b0000000000100000);
        REQUIRE(fun("instance", 8) == 0b0001100100010101);
        REQUIRE(fun("aaaaa", 5) == 0b0000000000110000);
        REQUIRE(fun("ebiz", 4) == 0b0000000100000001);
    }
}

TEST_CASE("is calculating count fingerprint for mixed letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 1 -- count, letters type 1 - mixed (etaokvbp).
        Fingerprints<FING_T> fingerprints(distanceType, 1, 1);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
        REQUIRE(fun("ala", 3) == 0b0000000000100000);
        REQUIRE(fun("instance", 8) == 0b0000000000010101);
        REQUIRE(fun("aaaaa", 5) == 0b0000000000110000);
        REQUIRE(fun("ebiz", 4) == 0b0001000000000001);
    }
}

TEST_CASE("is calculating count fingerprint for rare letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 1 -- count, letters type 2 - rare (zqxjkvbp).
        Fingerprints<FING_T> fingerprints(distanceType, 1, 2);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
        REQUIRE(fun("ala", 3) == 0b0000000000000000);
        REQUIRE(fun("instance", 8) == 0b0000000000000000);
        REQUIRE(fun("aaaaa", 5) == 0b0000000000000000);
        REQUIRE(fun("ebiz", 4) == 0b0001000000000001);
    }
}

TEST_CASE("is calculating position fingerprint for common letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 2 -- position, letters type 0 - common (etaoin).
        Fingerprints<FING_T> fingerprints(distanceType, 2, 0);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
        REQUIRE(fun("ala", 3) == 0b0111111000111111);
        REQUIRE(fun("instance", 8) == 0b1000111100011111);
        REQUIRE(fun("aaaaa", 5) == 0b0111111000111111);
        REQUIRE(fun("ebiz", 4) == 0b0010111111111000);
    }
}

TEST_CASE("is calculating position fingerprint for mixed letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 2 -- position, letters type 1 - mixed (etakvb).
        Fingerprints<FING_T> fingerprints(distanceType, 2, 1);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
        REQUIRE(fun("ala", 3) == 0b0111111000111111);
        REQUIRE(fun("instance", 8) == 0b0111111100011111);
        REQUIRE(fun("aaaaa", 5) == 0b0111111000111111);
        REQUIRE(fun("ebiz", 4) == 0b1111111111111000);
    }
}

TEST_CASE("is calculating position fingerprint for rare letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 2 -- position, letters type 2 - rare (zqxjkv).
        Fingerprints<FING_T> fingerprints(distanceType, 2, 2);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
        REQUIRE(fun("ala", 3) == 0b0111111111111111);
        REQUIRE(fun("instance", 8) == 0b0111111111111111);
        REQUIRE(fun("aaaaa", 5) == 0b0111111111111111);
        REQUIRE(fun("ebiz", 4) == 0b0111111111111011);
    }
}

TEST_CASE("is calculating occurrence halved fingerprint for common letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 3 -- occurrence halved, letters type 0 - common (etaoinsh).
        Fingerprints<FING_T> fingerprints(distanceType, 3, 0);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
        REQUIRE(fun("ala", 3) == 0b0000000000110000);
        REQUIRE(fun("instance", 8) == 0b0001110100100110);
        REQUIRE(fun("aaaaa", 5) == 0b0000000000110000);
        REQUIRE(fun("ebiz", 4) == 0b0000001000000001);
    }
}

TEST_CASE("is calculating occurrence halved fingerprint for mixed letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 3 -- occurrence halved, letters type 1 - mixed (etaokvbp).
        Fingerprints<FING_T> fingerprints(distanceType, 3, 1);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
        REQUIRE(fun("ala", 3) == 0b0000000000110000);
        REQUIRE(fun("instance", 8) == 0b0000000000100110);
        REQUIRE(fun("aaaaa", 5) == 0b0000000000110000);
        REQUIRE(fun("ebiz", 4) == 0b0001000000000001);
    }
}

TEST_CASE("is calculating occurrence halved fingerprint for rare letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 3 -- occurrence halved, letters type 2 - rare (zqxjkvbp).
        Fingerprints<FING_T> fingerprints(distanceType, 3, 2);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        // Fingerprints are constructed from the right-hand side (i.e. from the least significant bit).
        REQUIRE(fun("ala", 3) == 0b0000000000000000);
        REQUIRE(fun("instance", 8) == 0b0000000000000000);
        REQUIRE(fun("aaaaa", 5) == 0b0000000000000000);
        REQUIRE(fun("ebiz", 4) == 0b0001000000000010);
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
    
    REQUIRE(FingerprintsWhitebox::calcHammingWeight<FING_T>(0b001111100000) == 5);
    REQUIRE(FingerprintsWhitebox::calcHammingWeight<FING_T>(0b000111010110) == 6);
    REQUIRE(FingerprintsWhitebox::calcHammingWeight<FING_T>(0b111101010010) == 7);

    REQUIRE(FingerprintsWhitebox::calcHammingWeight<FING_T>(65535) == 16); // 2^16 - 1
}

TEST_CASE("is calculcating number of errors for occurrence fingerprint for common letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 0 -- occurrence, letters type 0 - common (etaoinshrdlcumwf).
        Fingerprints<FING_T> fingerprints(distanceType, 0, 0);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        string str1 = "ala ma kota";
        string str2 = "ala ma koto";
        string str3 = "ala ma kkkk";
        string str4 = "bla ma kota";

        FING_T f1 = fun(str1.c_str(), str1.size());
        FING_T f2 = fun(str2.c_str(), str2.size());
        FING_T f3 = fun(str3.c_str(), str3.size());
        FING_T f4 = fun(str4.c_str(), str4.size());

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f1) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f4) == 0);
        
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f4) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f4) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f2) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f3) == 1);
    }
}

TEST_CASE("is calculcating number of errors for occurrence fingerprint for mixed letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 0 -- occurrence, letters type 1 - mixed (etaoinshzqxjkvbp).
        Fingerprints<FING_T> fingerprints(distanceType, 0, 1);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        string str1 = "ala ma kota";
        string str2 = "ala ma koto";
        string str3 = "ala ma kkkk";
        string str4 = "bla ma kota";

        FING_T f1 = fun(str1.c_str(), str1.size());
        FING_T f2 = fun(str2.c_str(), str2.size());
        FING_T f3 = fun(str3.c_str(), str3.size());
        FING_T f4 = fun(str4.c_str(), str4.size());

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f1) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f4) == 0);
        
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f4) == 2);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f3) == 2);
    }
}

TEST_CASE("is calculcating number of errors for occurrence fingerprint for rare letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 0 -- occurrence, letters type 2 - rare (zqxjkvbpygfwmucl).
        Fingerprints<FING_T> fingerprints(distanceType, 0, 2);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        string str1 = "ala ma kota";
        string str2 = "ala ma koto";
        string str3 = "ala ma kkkk";
        string str4 = "bla ma kota";

        FING_T f1 = fun(str1.c_str(), str1.size());
        FING_T f2 = fun(str2.c_str(), str2.size());
        FING_T f3 = fun(str3.c_str(), str3.size());
        FING_T f4 = fun(str4.c_str(), str4.size());

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f1) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f4) == 0);
        
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f2) == 0);
        
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f3) == 1);
    }
}

TEST_CASE("is calculating number of errors for count fingerprint for common letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 1 -- count, letters type 0 - common (etaoinsh).
        Fingerprints<FING_T> fingerprints(distanceType, 1, 0);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        string str1 = "ala ma kota";
        string str2 = "ala ma koto";
        string str3 = "ala ma kkkk";
        string str4 = "bla ma kota";

        FING_T f1 = fun(str1.c_str(), str1.size());
        FING_T f2 = fun(str2.c_str(), str2.size());
        FING_T f3 = fun(str3.c_str(), str3.size());
        FING_T f4 = fun(str4.c_str(), str4.size());

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f1) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f4) == 0);
        
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f2) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f4) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f3) == 1);
    }
}

TEST_CASE("is calculating number of errors for count fingerprint for mixed letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 1 -- count, letters type 1 - mixed (etaokvbp).
        Fingerprints<FING_T> fingerprints(distanceType, 1, 1);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        string str1 = "ala ma kota";
        string str2 = "ala ma koto";
        string str3 = "ala ma kkkk";
        string str4 = "bla ma kota";

        FING_T f1 = fun(str1.c_str(), str1.size());
        FING_T f2 = fun(str2.c_str(), str2.size());
        FING_T f3 = fun(str3.c_str(), str3.size());
        FING_T f4 = fun(str4.c_str(), str4.size());

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f1) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f4) == 0);
        
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f2) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f3) == 2);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f1) == 2);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f3) == 2);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f2) == 2);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f4) == 2);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f3) == 2);
    }
}

TEST_CASE("is calculating number of errors for count fingerprint for rare letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 1 -- count, letters type 2 - rare (zqxjkvbp).
        Fingerprints<FING_T> fingerprints(distanceType, 1, 2);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        string str1 = "ala ma kota";
        string str2 = "ala ma koto";
        string str3 = "ala ma kkkk";
        string str4 = "bla ma kota";

        FING_T f1 = fun(str1.c_str(), str1.size());
        FING_T f2 = fun(str2.c_str(), str2.size());
        FING_T f3 = fun(str3.c_str(), str3.size());
        FING_T f4 = fun(str4.c_str(), str4.size());

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f1) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f4) == 0);
        
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f3) == 1);
    }
}

TEST_CASE("is calculating number of errors for position fingerprint for common letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 2 -- position, letters type 0 - common (etaoin).
        Fingerprints<FING_T> fingerprints(distanceType, 2, 0);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        string str1 = "kota ma ala";
        string str2 = "koto ma ala";
        string str3 = "kkkk ma ala";
        string str4 = "kota ma bla";

        FING_T f1 = fun(str1.c_str(), str1.size());
        FING_T f2 = fun(str2.c_str(), str2.size());
        FING_T f3 = fun(str3.c_str(), str3.size());
        FING_T f4 = fun(str4.c_str(), str4.size());
        
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f1) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f4) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f2) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f3) == 2);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f1) == 2);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f4) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f4) == 2);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f3) == 2);
    }
}

TEST_CASE("is calculating number of errors for position fingerprint for mixed letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 2 -- position, letters type 1 - mixed (etakvb).
        Fingerprints<FING_T> fingerprints(distanceType, 2, 1);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);
    
        string str1 = "kota ma ala";
        string str2 = "koto ma ala";
        string str3 = "kkkk ma ala";
        string str4 = "kota ma bla";

        FING_T f1 = fun(str1.c_str(), str1.size());
        FING_T f2 = fun(str2.c_str(), str2.size());
        FING_T f3 = fun(str3.c_str(), str3.size());
        FING_T f4 = fun(str4.c_str(), str4.size());
        
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f1) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f4) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f2) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f4) == 2);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f3) == 2);
    }
}

TEST_CASE("is calculating number of errors for position fingerprint for rare letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 2 -- position, letters type 2 - rare (zqxjkv).
        Fingerprints<FING_T> fingerprints(distanceType, 2, 2);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        string str1 = "kota ma ala";
        string str2 = "koto ma ala";
        string str3 = "kkkk ma ala";
        string str4 = "kota ma bla";

        FING_T f1 = fun(str1.c_str(), str1.size());
        FING_T f2 = fun(str2.c_str(), str2.size());
        FING_T f3 = fun(str3.c_str(), str3.size());
        FING_T f4 = fun(str4.c_str(), str4.size());
        
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f1) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f4) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f2) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f4) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f4) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f2) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f4) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f3) == 0);
    }
}

TEST_CASE("is calculating number of errors for occurrence halved fingerprint for common letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 3 -- occurrence halved, letters type 0 - common (etaoinsh).
        Fingerprints<FING_T> fingerprints(distanceType, 3, 0);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        string str1 = "ala ma kota"; // ala m|a kota
        string str2 = "ala ma koto";
        string str3 = "ala ma kkkk";
        string str4 = "bla ma kota";

        FING_T f1 = fun(str1.c_str(), str1.size());
        FING_T f2 = fun(str2.c_str(), str2.size());
        FING_T f3 = fun(str3.c_str(), str3.size());
        FING_T f4 = fun(str4.c_str(), str4.size());

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f1) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f4) == 0);
        
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f4) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f4) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f2) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f3) == 1);
    }
}

TEST_CASE("is calculating number of errors for occurrence halved fingerprint for mixed letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 3 -- occurrence halved, letters type 1 - mixed (etaokvbp).
        Fingerprints<FING_T> fingerprints(distanceType, 3, 1);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        string str1 = "ala ma kota"; // ala m|a kota
        string str2 = "ala ma koto";
        string str3 = "ala ma kkkk";
        string str4 = "bla ma kota";

        FING_T f1 = fun(str1.c_str(), str1.size());
        FING_T f2 = fun(str2.c_str(), str2.size());
        FING_T f3 = fun(str3.c_str(), str3.size());
        FING_T f4 = fun(str4.c_str(), str4.size());

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f1) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f4) == 0);
        
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f3) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f4) == 2);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f3) == 2);
    }
}

TEST_CASE("is calculating number of errors for occurrence halved fingerprint for rare letters correct", "[fingerprints]")
{
    for (int distanceType : distanceTypes)
    {
        // Passing fingerprint type 3 -- occurrence halved, letters type 2 - rare (zqxjkvbp).
        Fingerprints<FING_T> fingerprints(distanceType, 3, 2);
        auto fun = FingerprintsWhitebox::getCalcFingerprintFun(fingerprints);

        string str1 = "ala ma kota"; // ala m|a kota
        string str2 = "ala ma koto";
        string str3 = "ala ma kkkk";
        string str4 = "bla ma kota";

        FING_T f1 = fun(str1.c_str(), str1.size());
        FING_T f2 = fun(str2.c_str(), str2.size());
        FING_T f3 = fun(str3.c_str(), str3.size());
        FING_T f4 = fun(str4.c_str(), str4.size());

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f1) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f4) == 0);
        
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f2) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f1) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f3) == 0);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f2) == 0);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f1, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f1) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f2, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f2) == 1);

        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f3, f4) == 1);
        REQUIRE(FingerprintsWhitebox::calcNErrors(fingerprints, f4, f3) == 1);
    }
}

} // namespace fingerprints
